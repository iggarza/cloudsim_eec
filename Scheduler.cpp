#include "Scheduler.hpp"
#include <vector>
#include <set>

static bool migrating = false;
static int total_machines;

void Scheduler::Init() {
    total_machines = Machine_GetTotal();
    SimOutput("Scheduler::Init(): Total number of machines is " + to_string(total_machines), 1);
    SimOutput("Scheduler::Init(): Initializing scheduler", 1);
    
    //sets all of the machines to the on state originally
    for (int i = 0; i < total_machines; i++) {
        Machine_SetState(MachineId_t(i), S0);
        machines.push_back(MachineId_t(i));
    }
    SimOutput("Scheduler::Init(): scheduler initialized with no active machines", 1);
    }

void Scheduler::MigrationComplete(Time_t time, VMId_t vm_id) {
    // Update your data structure. The VM now can receive new tasks
    migrating = false;
}

bool Scheduler::CheckMachineCompatibility(MachineInfo_t m_info, TaskInfo_t task_info) {
    if (m_info.cpu == task_info.required_cpu && (m_info.gpus == task_info.gpu_capable || (m_info.gpus && !task_info.gpu_capable))
        && (m_info.memory_size - m_info.memory_used >= task_info.required_memory + 8)) {
        return true;
    }
    return false;
}

void Scheduler::NewTask(Time_t now, TaskId_t task_id) {
   
    // Get the task parameters
    TaskInfo_t task_info = GetTaskInfo(task_id);

    //loop through existing on VMS
    for (auto& vm : vms) {
        SimOutput("looking at vm " + to_string(vm), 1);
        VMInfo_t vm_info = VM_GetInfo(vm);
        MachineInfo_t machine_info = Machine_GetInfo(vm_info.machine_id);
        //if the number of tasks on a given vm is less than the # of cores and if the number of tasks on a given machine
        //is less than the number of cores (each machine can have multiple vms)
        //3 is just a number that I thought would be a solid multiple without overwhelming the machines
        if (vm_info.active_tasks.size() < machine_info.num_cpus * 3 && machine_info.active_tasks < machine_info.num_cpus * 3) {
            //cpus match, vms match, gpu is compatible, and there is sufficient memory on the machine
            if (vm_info.cpu == task_info.required_cpu && vm_info.vm_type == task_info.required_vm 
            && (machine_info.gpus == task_info.gpu_capable || (machine_info.gpus && !task_info.gpu_capable))
            && ((machine_info.memory_size - machine_info.memory_used) >= task_info.required_memory + 8)) {
                SimOutput("Scheduler::NewTask(): A new task was added to an existing vm on a running machine", 1);
                SimOutput("adding to vm " + to_string(vm), 1);
                VM_AddTask(vm, task_id, task_info.priority);
                return;
            }
        }  
    }

    //no available vms to place a task on so create one to either run on an on machine or an off machine
    VMId_t new_vm = VM_Create(task_info.required_vm, task_info.required_cpu);
    vms.push_back(new_vm);

    //looping through machines that are on
    for (auto& machine : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine);
        SimOutput("machine " + to_string(machine), 1);
        if (machine_info.s_state == S0) {
            //3 is just a number that I thought would be a solid multiple without overwhelming the machines
            if (machine_info.active_tasks < (machine_info.num_cpus * 3) && CheckMachineCompatibility(machine_info, task_info)) {
                //create the vm to attach to the machine
                VM_Attach(new_vm, machine);
                VM_AddTask(new_vm, task_id, GetTaskInfo(task_id).priority);
                SimOutput("Scheduler::NewTask(): A new vm has been created on machine " + to_string(machine), 1);
                return;
            }
        }
    }

    //finding the first machine that is off, has the matching cpu type, gpu, and the memory is big enough
    for (auto& machine : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine);
        SimOutput("machine " + to_string(machine), 1);
        if (machine_info.s_state != S0 && CheckMachineCompatibility(machine_info, task_info)) {
                    SimOutput("NewTask(): A new machine is being woken up to handle task", 1);
            Machine_SetState(machine, S0);
            machines.push_back(machine);
            VM_Attach(new_vm, machine);
            VM_AddTask(new_vm, task_id, GetTaskInfo(task_id).priority);
            SimOutput("Scheduler::NewTask(): A new vm has been attached to machine " + to_string(machine), 1);
            return;
        }
    }
    SimOutput("Scheduler::NewTask(): A compatible machine was not found, task left unassigned", 1);
}

void Scheduler::PeriodicCheck(Time_t now) {
    // This method should be called from SchedulerCheck()
    // SchedulerCheck is called periodically by the simulator to allow you to monitor, make decisions, adjustments, etc.
    // Unlike the other invocations of the scheduler, this one doesn't report any specific event
    // Recommendation: Take advantage of this function to do some monitoring and adjustments as necessary
    for (auto& machine : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine);
        //if a machine doesn't have any active tasks, turn it off
        if (machine_info.active_tasks == 0) {
            Machine_SetState(machine, S5);
        }
    }
}

void Scheduler::Shutdown(Time_t time) {
    // Do your final reporting and bookkeeping here.
    // Report about the total energy consumed
    // Report about the SLA compliance
    // Shutdown everything to be tidy :-)
    for(auto & vm: vms) {
        VM_Shutdown(vm);
    }
    for(auto & machine: machines) {
        Machine_SetState(machine, S5);
    }
    SimOutput("SimulationComplete(): Finished!", 4);
    SimOutput("SimulationComplete(): Time is " + to_string(time), 4);
}

void Scheduler::TaskComplete(Time_t now, TaskId_t task_id) {
    // Do any bookkeeping necessary for the data structures
    // Decide if a machine is to be turned off, slowed down, or VMs to be migrated according to your policy
    // This is an opportunity to make any adjustments to optimize performance/energy
    SimOutput("Scheduler::TaskComplete(): Task " + to_string(task_id) + " is complete at " + to_string(now), 4);
}

// Public interface below

static Scheduler Scheduler;

void InitScheduler() {
    SimOutput("InitScheduler(): Initializing scheduler", 4);
    Scheduler.Init();
}

void HandleNewTask(Time_t time, TaskId_t task_id) {
    SimOutput("HandleNewTask(): Received new task " + to_string(task_id) + " at time " + to_string(time), 4);
    Scheduler.NewTask(time, task_id);
}

void HandleTaskCompletion(Time_t time, TaskId_t task_id) {
    SimOutput("HandleTaskCompletion(): Task " + to_string(task_id) + " completed at time " + to_string(time), 4);
    Scheduler.TaskComplete(time, task_id);
}

void MemoryWarning(Time_t time, MachineId_t machine_id) {
    // The simulator is alerting you that machine identified by machine_id is overcommitted
    SimOutput("MemoryWarning(): Overflow at " + to_string(machine_id) + " was detected at time " + to_string(time), 0);
}

void MigrationDone(Time_t time, VMId_t vm_id) {
    // The function is called on to alert you that migration is complete
    SimOutput("MigrationDone(): Migration of VM " + to_string(vm_id) + " was completed at time " + to_string(time), 4);
    Scheduler.MigrationComplete(time, vm_id);
    migrating = false;
}

void SchedulerCheck(Time_t time) {
    // This function is called periodically by the simulator, no specific event
    // SimOutput("SchedulerCheck(): SchedulerCheck() called at " + to_string(time), 4);
    // Scheduler.PeriodicCheck(time);
    // static unsigned counts = 0;
    // counts++;
    // if(counts == 10) {
    //     migrating = true;
    //     VM_Migrate(1, 9);
    // }
    // Scheduler.PeriodicCheck(time);
}

void SimulationComplete(Time_t time) {
    // This function is called before the simulation terminates Add whatever you feel like.
    cout << "SLA violation report" << endl;
    cout << "SLA0: " << GetSLAReport(SLA0) << "%" << endl;
    cout << "SLA1: " << GetSLAReport(SLA1) << "%" << endl;
    cout << "SLA2: " << GetSLAReport(SLA2) << "%" << endl;     // SLA3 do not have SLA violation issues
    cout << "Total Energy " << Machine_GetClusterEnergy() << "KW-Hour" << endl;
    cout << "Simulation run finished in " << double(time)/1000000 << " seconds" << endl;
    SimOutput("SimulationComplete(): Simulation finished at time " + to_string(time), 4);
    
    Scheduler.Shutdown(time);
}

void SLAWarning(Time_t time, TaskId_t task_id) {
    
}

void StateChangeComplete(Time_t time, MachineId_t machine_id) {
    // Called in response to an earlier request to change the state of a machine
}


