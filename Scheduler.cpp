// //
// //  Scheduler.cpp
// //  CloudSim
// //
// //  Created by ELMOOTAZBELLAH ELNOZAHY on 10/20/24.
// //

// #include "Scheduler.hpp"
// #include <vector>

// static bool migrating = false;
// static int total_machines = 0;
// vector<MachineId_t> off_machines;

// void Scheduler::Init() {
//     // Find the parameters of the clusters
//     // Get the total number of machines
//     // For each machine:
//     //      Get the type of the machine
//     //      Get the memory of the machine
//     //      Get the number of CPUs
//     //      Get if there is a GPU or not
//     // 
//     total_machines = Machine_GetTotal();
//     SimOutput("Scheduler::Init(): Total number of machines is " + to_string(total_machines), 1);
//     SimOutput("Scheduler::Init(): Initializing scheduler", 1);
    
//     //turning all machines on 
//     for (int i = 0; i < total_machines; i++) {
//         Machine_SetState(MachineId_t(i), S0);
//         //off_machines.push_back(MachineId_t(i));
//         machines.push_back(MachineId_t(i));
//     }
//     SimOutput("Scheduler::Init(): scheduler initialized with no active machines", 1);
// }

// void Scheduler::MigrationComplete(Time_t time, VMId_t vm_id) {
//     // Update your data structure. The VM now can receive new tasks
//     migrating = false;
// }

// void Scheduler::NewTask(Time_t now, TaskId_t task_id) {
//     // Get the task parameters
//     bool needs_gpu = IsTaskGPUCapable(task_id);
//     unsigned req_memory = GetTaskMemory(task_id);
//     VMType_t req_vm = RequiredVMType(task_id);
//     CPUType_t req_cpu = RequiredCPUType(task_id);

//     //loop through existing on VMS
//     for (auto& vm : vms) {
//         SimOutput("looking at vm " + to_string(vm), 1);
//         VMInfo_t vm_info = VM_GetInfo(vm);
//         MachineInfo_t m_info = Machine_GetInfo(vm_info.machine_id);
//         //if the number of tasks on a given vm is less than the # of cores and if the number of tasks on a given machine
//         //is less than the number of cores (each machine can have multiple vms)
//         if (vm_info.active_tasks.size() < m_info.num_cpus && m_info.active_tasks <= m_info.num_cpus) {
//             SimOutput("cores available", 1);
//             //cpus match, vms match, gpu is compatible, and there is sufficient memory on the machine
//             if (vm_info.cpu == req_cpu && vm_info.vm_type == req_vm && (m_info.gpus == needs_gpu || (m_info.gpus && !needs_gpu))
//             && ((m_info.memory_size - m_info.memory_used) >= req_memory + 8)) { //and that there is still memory available on the vm but idk how to write that
//                 SimOutput("Scheduler::NewTask(): A new task was added to an existing vm on a running machine", 1);
//                 SimOutput("adding to vm " + to_string(vm), 1);
//                 VM_AddTask(vm, task_id, GetTaskInfo(task_id).priority);
//                 return;
//             }
//         }  
//     }
//     //loop through on machines to attack a vm to
//     VMId_t new_vm = VM_Create(req_vm, req_cpu);
//     vms.push_back(new_vm);
//     for (auto& machine : machines) {
//         MachineInfo_t m_info = Machine_GetInfo(machine);
//         SimOutput("machine " + to_string(machine), 1);
//         if (m_info.s_state == S0) {
//         //finding the first machine that, has the matching cpu type, gpu, and the memory is big enough
//             if (m_info.cpu == req_cpu && (m_info.gpus == needs_gpu || (m_info.gpus && !needs_gpu))
//                     && ((m_info.memory_size - m_info.memory_used) >= (req_memory + 8))) {
//                 VM_Attach(new_vm, machine);
//                 VM_AddTask(new_vm, task_id, GetTaskInfo(task_id).priority);
//                 SimOutput("Scheduler::NewTask(): A new vm has been created on machine " + to_string(machine), 1);
//                 return;
//             }
//         }
//     }

//      // If no machine is available, turn on a powered-down machine
//     for (MachineId_t machine : machines) {
//         MachineInfo_t m_info = Machine_GetInfo(machine);
//         if (m_info.s_state == S5) {
//             if (m_info.cpu == req_cpu && (m_info.gpus == needs_gpu || (m_info.gpus && !needs_gpu))
//                     && ((m_info.memory_size - m_info.memory_used) >= (req_memory + 8))) {
//                 SimOutput("NewTask(): A new machine is being woken up to handle task", 1);
//                 Machine_SetState(machine, S0);
//                 VM_Attach(new_vm, machine);
//                 VM_AddTask(new_vm, task_id, GetTaskInfo(task_id).priority);
//                 SimOutput("Task " + to_string(task_id) + " assigned to new VM " + to_string(new_vm) + " on powered-up Machine " + to_string(machine), 2);
//                 return;
//             }
//         }
//     }



//     SimOutput("Scheduler::NewTask(): A compatible machine was not found, task left unassigned", 1);
// }


//     // Decide to attach the task to an existing VM, 
//     //      vm.AddTask(taskid, Priority_T priority); or
//     // Create a new VM, attach the VM to a machine
//     //      VM vm(type of the VM)
//     //      vm.Attach(machine_id);
//     //      vm.AddTask(taskid, Priority_t priority) or
//     // Turn on a machine, create a new VM, attach it to the VM, then add the task
//     //
//     // Turn on a machine, migrate an existing VM from a loaded machine....
//     //
//     // Other possibilities as desired
//     // Priority_t priority = (task_id == 0 || task_id == 64)? HIGH_PRIORITY : MID_PRIORITY;
//     // if(migrating) {
//     //     VM_AddTask(vms[0], task_id, priority);
//     // }
//     // else {
//     //     VM_AddTask(vms[task_id % active_machines], task_id, priority);
//     //} Skeleton code, you need to change it according to your algorithm


// void Scheduler::PeriodicCheck(Time_t now) {
//     // This method should be called from SchedulerCheck()
//     // SchedulerCheck is called periodically by the simulator to allow you to monitor, make decisions, adjustments, etc.
//     // Unlike the other invocations of the scheduler, this one doesn't report any specific event
//     // Recommendation: Take advantage of this function to do some monitoring and adjustments as necessary
//     // for (auto& vm : vms) {
//     //     VMInfo_t vm_info = VM_GetInfo(vm);
//     //     for (auto& task : v_info.active_tasks) {
//     //         TaskInfo_t task_info = GetTaskInfo(task);
//     //         Time_t finish_by = task_info.target_completion;
//     //         if (now > finish_by) {
//     //             //migrate to a better machine
//     //         }
//     //     }
//     // }  
//     // int index = 0;
//     // for (auto& machine : machines) {
//     //     index++;
//     //     MachineInfo_t m_info = Machine_GetInfo(machine);
//     //     if (m_info.active_tasks == 0) {
//     //         Machine_SetState(machine, S5);
//     //         off_machines.push_back(machine);
//     //     }
//     // }
// }

// // void Scheduler::MigrateToBetter(TaskId_t task_id, VMId_t vm_id){
// //     //check for a better vm or machine 
// //     for (auto& curr_vm : vms) {
// //         //want to make sure it's not the same vm
// //         if (curr_vm != vm_id) {
// //             VMInfo_t curr_vm_info = VM_GetInfo(curr_vm);
// //             if (curr_vm_info.cpu == )
// //         }
// //     }
// // }

// void Scheduler::Shutdown(Time_t time) {
//     // Do your final reporting and bookkeeping here.
//     // Report about the total energy consumed
//     // Report about the SLA compliance
//     // Shutdown everything to be tidy :-)
//     for(auto & vm: vms) {
//         VM_Shutdown(vm);
//     }
//     // for(auto & machine: machines) {
//     //     Machine_SetState(machine, S5);
//     // }
//     // for(auto & machine: off_machines) {
//     //     Machine_SetState(machine, S5);
//     // }
//     SimOutput("SimulationComplete(): Finished!", 4);
//     SimOutput("SimulationComplete(): Time is " + to_string(time), 4);
// }

// void Scheduler::TaskComplete(Time_t now, TaskId_t task_id) {
    
//     // Do any bookkeeping necessary for the data structures
//     // Decide if a machine is to be turned off, slowed down, or VMs to be migrated according to your policy
//     // This is an opportunity to make any adjustments to optimize performance/energy
//     SimOutput("Scheduler::TaskComplete(): Task " + to_string(task_id) + " is complete at " + to_string(now), 4);
// }

// // Public interface below

// static Scheduler Scheduler;

// void InitScheduler() {
//     SimOutput("InitScheduler(): Initializing scheduler", 4);
//     Scheduler.Init();
// }

// void HandleNewTask(Time_t time, TaskId_t task_id) {
//     SimOutput("HandleNewTask(): Received new task " + to_string(task_id) + " at time " + to_string(time), 4);
//     Scheduler.NewTask(time, task_id);
// }

// void HandleTaskCompletion(Time_t time, TaskId_t task_id) {
//     SimOutput("HandleTaskCompletion(): Task " + to_string(task_id) + " completed at time " + to_string(time), 4);
//     Scheduler.TaskComplete(time, task_id);
// }

// void MemoryWarning(Time_t time, MachineId_t machine_id) {
//     // The simulator is alerting you that machine identified by machine_id is overcommitted
//     SimOutput("MemoryWarning(): Overflow at " + to_string(machine_id) + " was detected at time " + to_string(time), 0);
// }

// void MigrationDone(Time_t time, VMId_t vm_id) {
//     // The function is called on to alert you that migration is complete
//     SimOutput("MigrationDone(): Migration of VM " + to_string(vm_id) + " was completed at time " + to_string(time), 4);
//     Scheduler.MigrationComplete(time, vm_id);
//     migrating = false;
// }

// void SchedulerCheck(Time_t time) {
//     // // This function is called periodically by the simulator, no specific event
//     // SimOutput("SchedulerCheck(): SchedulerCheck() called at " + to_string(time), 4);
//     // Scheduler.PeriodicCheck(time);
//     // static unsigned counts = 0;
//     // counts++;
//     // if(counts == 10) {
//     //     migrating = true;
//     //     VM_Migrate(1, 9);
//     //}
//     // Scheduler.PeriodicCheck(time);
// }

// void SimulationComplete(Time_t time) {
//     // This function is called before the simulation terminates Add whatever you feel like.
//     cout << "SLA violation report" << endl;
//     cout << "SLA0: " << GetSLAReport(SLA0) << "%" << endl;
//     cout << "SLA1: " << GetSLAReport(SLA1) << "%" << endl;
//     cout << "SLA2: " << GetSLAReport(SLA2) << "%" << endl;     // SLA3 do not have SLA violation issues
//     cout << "Total Energy " << Machine_GetClusterEnergy() << "KW-Hour" << endl;
//     cout << "Simulation run finished in " << double(time)/1000000 << " seconds" << endl;
//     SimOutput("SimulationComplete(): Simulation finished at time " + to_string(time), 4);
    
//     Scheduler.Shutdown(time);
// }

// void SLAWarning(Time_t time, TaskId_t task_id) {
    
// }

// void StateChangeComplete(Time_t time, MachineId_t machine_id) {
//     // Called in response to an earlier request to change the state of a machine
// }

//
//  Scheduler.cpp
//  CloudSim
//
//  Created by ELMOOTAZBELLAH ELNOZAHY on 10/20/24.
//

#include "Scheduler.hpp"
#include <vector>
#include <set>

static bool migrating = false;
static int total_machines = 0;
// vector<MachineId_t> on_machines;
vector<MachineId_t> off_machines;

void Scheduler::Init() {
    // Find the parameters of the clusters
    // Get the total number of machines
    // For each machine:
    //      Get the type of the machine
    //      Get the memory of the machine
    //      Get the number of CPUs
    //      Get if there is a GPU or not
    // 
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

void Scheduler::NewTask(Time_t now, TaskId_t task_id) {
    // Get the task parameters
    bool needs_gpu = IsTaskGPUCapable(task_id);
    unsigned req_memory = GetTaskMemory(task_id);
    VMType_t req_vm = RequiredVMType(task_id);
    CPUType_t req_cpu = RequiredCPUType(task_id);

    //loop through existing on VMS
    for (auto& vm : vms) {
        SimOutput("looking at vm " + to_string(vm), 1);
        VMInfo_t vm_info = VM_GetInfo(vm);
        MachineInfo_t machine_info = Machine_GetInfo(vm_info.machine_id);
        //if the number of tasks on a given vm is less than the # of cores and if the number of tasks on a given machine
        //is less than the number of cores (each machine can have multiple vms)
        if (vm_info.active_tasks.size() < machine_info.num_cpus && machine_info.active_tasks < machine_info.num_cpus) {
            //cpus match, vms match, gpu is compatible, and there is sufficient memory on the machine
            if (vm_info.cpu == req_cpu && vm_info.vm_type == req_vm && (machine_info.gpus == needs_gpu || (machine_info.gpus && !needs_gpu))
            && ((machine_info.memory_size - machine_info.memory_used) >= req_memory + 8)) { //and that there is still memory available on the vm but idk how to write that
                SimOutput("Scheduler::NewTask(): A new task was added to an existing vm on a running machine", 1);
                SimOutput("adding to vm " + to_string(vm), 1);
                VM_AddTask(vm, task_id, GetTaskInfo(task_id).priority);
                return;
            }
        }  
    }
    // no available vms to place a task on so create one to either run on an on machine or an off machine
    VMId_t new_vm = VM_Create(req_vm, req_cpu);
    vms.push_back(new_vm);
    for (auto& machine : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine);
        SimOutput("machine " + to_string(machine), 1);
        if (machine_info.s_state == S0) {
            if (machine_info.active_tasks < machine_info.num_cpus) {
                if (machine_info.cpu == req_cpu && (machine_info.gpus == needs_gpu || (machine_info.gpus && !needs_gpu))
                        && (machine_info.memory_size - machine_info.memory_used >= req_memory + 8)) {
                    //create the vm to attach to the machine
                    VM_Attach(new_vm, machine);
                    VM_AddTask(new_vm, task_id, GetTaskInfo(task_id).priority);
                    SimOutput("Scheduler::NewTask(): A new vm has been created on machine " + to_string(machine), 1);
                    return;
                }
            }
        }
    }
    //finding the first machine that is off, has the matching cpu type, gpu, and the memory is big enough
    for (auto& machine : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine);
        SimOutput("machine " + to_string(machine), 1);
        if (machine_info.s_state != S0) {
            if (machine_info.active_tasks < machine_info.num_cpus) {
                if (machine_info.cpu == req_cpu && (machine_info.gpus == needs_gpu || (machine_info.gpus && !needs_gpu))
                        && (machine_info.memory_size - machine_info.memory_used >= req_memory + 8)) {
                    Machine_SetState(machine, S0);
                    machines.push_back(machine);
                    SimOutput("NewTask(): A new machine is being woken up to handle task", 1);
                    //create the vm to attach to the machine
                    VM_Attach(new_vm, machine);
                    VM_AddTask(new_vm, task_id, GetTaskInfo(task_id).priority);
                    SimOutput("Scheduler::NewTask(): A new vm has been created on machine " + to_string(machine), 1);
                    return;
                }
            }
        }
    }
    SimOutput("Scheduler::NewTask(): A compatible machine was not found, task left unassigned", 1);



    // Decide to attach the task to an existing VM, 
    //      vm.AddTask(taskid, Priority_T priority); or
    // Create a new VM, attach the VM to a machine
    //      VM vm(type of the VM)
    //      vm.Attach(machine_id);
    //      vm.AddTask(taskid, Priority_t priority) or
    // Turn on a machine, create a new VM, attach it to the VM, then add the task
    //
    // Turn on a machine, migrate an existing VM from a loaded machine....
    //
    // Other possibilities as desired
    // Priority_t priority = (task_id == 0 || task_id == 64)? HIGH_PRIORITY : MID_PRIORITY;
    // if(migrating) {
    //     VM_AddTask(vms[0], task_id, priority);
    // }
    // else {
    //     VM_AddTask(vms[task_id % active_machines], task_id, priority);
    //} Skeleton code, you need to change it according to your algorithm
}

void Scheduler::PeriodicCheck(Time_t now) {
    // This method should be called from SchedulerCheck()
    // SchedulerCheck is called periodically by the simulator to allow you to monitor, make decisions, adjustments, etc.
    // Unlike the other invocations of the scheduler, this one doesn't report any specific event
    // Recommendation: Take advantage of this function to do some monitoring and adjustments as necessary
    for (auto& machine : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine);
        if (machine_info.active_tasks == 0) {
            Machine_SetState(machine, S0);
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
    for(auto & machine: off_machines) {
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
    // // This function is called periodically by the simulator, no specific event
    // SimOutput("SchedulerCheck(): SchedulerCheck() called at " + to_string(time), 4);
    // Scheduler.PeriodicCheck(time);
    // static unsigned counts = 0;
    // counts++;
    // if(counts == 10) {
    //     migrating = true;
    //     VM_Migrate(1, 9);
    //}
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


