#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

constexpr int QUANTUM = 5;

struct Process {
    std::string id;
    int duration;
    int priority;
    int status;
    std::string command;
    std::string user;

    Process(std::string id, int duration, int priority, std::string command, std::string user)
        : id(id), duration(duration), priority(priority), status(0), command(command), user(user) {}
};

void simulateProcessExecution(Process &process) {
    std::cout << "Executando Processo: " << process.id << "\n"
              << "Tempo Restante: " << process.duration << " segundos\n"
              << "Prioridade: " << process.priority << "\n"
              << "Command: " << process.command << "\n"
              << "Usuario: " << process.user << "\n";

    if (process.duration <= QUANTUM) {
        std::this_thread::sleep_for(std::chrono::seconds(process.duration));
        process.status = 1;
        process.duration = 0;
        std::cout << "Status: Processo " << process.id << " executado com exito!\n\n";
    } else {
        std::this_thread::sleep_for(std::chrono::seconds(QUANTUM));
        process.duration -= QUANTUM;
        std::cout << "Status: Processo " << process.id << " incompleto. Tempo restante: " << process.duration << " segundos\n\n";
    }
}

int main() {
    std::ifstream inputFile("processes.txt");
    if (!inputFile) {
        std::cerr << "Error: Arquivo nao pode ser aberto.\n";
        return 1;
    }

    std::string line;
    std::vector<Process> processes;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string id, durationStr, priorityStr, command, user;

        std::getline(iss, id, ';');
        std::getline(iss, durationStr, ';');
        std::getline(iss, priorityStr, ';');
        std::getline(iss, command, ';');
        std::getline(iss, user, ';');

        int duration = std::stoi(durationStr);
        int priority = std::stoi(priorityStr);

        processes.emplace_back(id, duration, priority, command, user);
    }
    
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.priority > b.priority;
    });

    while(true) {
        bool allCompleted = true;
        
        for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].status == 0) {
                allCompleted = false;
                simulateProcessExecution(processes[i]);
            }
        }
        
        if (allCompleted) break;
    }

    std::cout << "Todos os processos foram executados.\n";
    return 0;
}
