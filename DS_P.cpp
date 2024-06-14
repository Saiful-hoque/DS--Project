#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;

// User structure
struct User {
    string username;
    string password;
};

// Task structure
struct Task {
    int id;
    string username; // Username of the user who created the task
    string description;
    string deadline;
    bool completed;
    string completionDate;
};

// Task Manager class
class TaskManager {
private:
    string filename;
    string username;
    vector<Task> tasks;

public:
    TaskManager(const string& filename, const string& username) : filename(filename), username(username) {
        loadTasks();
    }

    // Add a task
    void addTask(const string& description, const string& deadline) {
        tasks.push_back({getNextId(), username, description, deadline, false, ""});
        saveTasks();
    }

    // Delete a task
    void deleteTask(int taskId) {
        auto it = findTaskById(taskId);
        if (it != tasks.end()) {
            tasks.erase(it);
            saveTasks();
        } else {
            cerr << "Task not found." << endl;
        }
    }

    // Complete a task
    void completeTask(int taskId) {
        auto it = findTaskById(taskId);
        if (it != tasks.end()) {
            it->completed = true;
            it->completionDate = getCurrentDate();
            saveTasks();
        } else {
            cerr << "Task not found." << endl;
        }
    }

    // Display tasks for the logged-in user
    void displayTasks() {
        if (tasks.empty()) {
            cout << "No tasks to display." << endl;
            return;
        }

        cout << "Tasks:" << endl;
        for (const auto& task : tasks) {
            if (task.username == username) {
                cout << "ID: " << task.id << ", Description: " << task.description << ", Deadline: " << task.deadline;
                if (task.completed) {
                    cout << ", Completed: Yes, Completion Date: " << task.completionDate;
                } else {
                    cout << ", Completed: No";
                }
                cout << endl;
            }
        }
    }

private:
    // Load tasks from file
    void loadTasks() {
        ifstream inputFile(filename);
        if (inputFile.is_open()) {
            string line;
            while (getline(inputFile, line)) {
                stringstream ss(line);
                Task task;
                ss >> task.id >> task.username >> task.description >> task.deadline >> task.completed >> task.completionDate;
                tasks.push_back(task);
            }
            inputFile.close();
        }
    }

    // Save tasks to file
    void saveTasks() {
        ofstream outputFile(filename);
        if (outputFile.is_open()) {
            for (const auto& task : tasks) {
                outputFile << task.id << " " << task.username << " " << task.description << " " << task.deadline << " " << task.completed << " " << task.completionDate << endl;
            }
            outputFile.close();
        } else {
            cerr << "Unable to open file for writing." << endl;
        }
    }

    // Get next available task ID
    int getNextId() {
        return tasks.empty() ? 1 : tasks.back().id + 1;
    }

    // Find task by ID
    vector<Task>::iterator findTaskById(int taskId) {
        return find_if(tasks.begin(), tasks.end(), [taskId](const Task& task) {
            return task.id == taskId;
        });
    }

    // Get current date as string
    string getCurrentDate() {
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d", timeinfo);
        return string(buffer);
    }
};

// Function to read users from file
vector<User> readUsersFromFile(const string& filename) {
    vector<User> users;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string username, password;
            ss >> username >> password;
            users.push_back({username, password});
        }
        file.close();
    } else {
        cerr << "Unable to open user file." << endl;
    }
    return users;
}

// Function to write users to file
void writeUsersToFile(const string& filename, const vector<User>& users) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& user : users) {
            file << user.username << " " << user.password << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open user file." << endl;
    }
}

// Check if username already exists
bool usernameExists(const string& username, const vector<User>& users) {
    return find_if(users.begin(), users.end(), [username](const User& user) {
        return user.username == username;
    }) != users.end();
}

// Main function
int main() {
    const string userFilename = "users.txt"; // File name for storing users
    const string taskFilename = "tasks.txt"; // File name for storing tasks

    // Read users from file
    vector<User> users = readUsersFromFile(userFilename);

    // Login or Register
    string username, password;
    bool loggedIn = false;

    while (!loggedIn) {
        cout << "1. Login\n2. Register\nEnter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;

                auto it = find_if(users.begin(), users.end(), [username, password](const User& user) {
                    return user.username == username && user.password == password;
                });

                if (it != users.end()) {
                    loggedIn = true;
                    cout << "Login successful." << endl;
                } else {
                    cout << "Login failed. Invalid username or password." << endl;
                }
                break;
            }
            case 2: {
                cout << "Enter username: ";
                cin >> username;

                if (usernameExists(username, users)) {
                    cout << "Username already exists. Please choose a different username." << endl;
                    break;
                }

                cout << "Enter password: ";
                cin >> password;

                users.push_back({username, password});
                cout << "Registration successful. Please login." << endl;
                break;
            }
            default:
                cout << "Invalid choice." << endl;
        }
    }

    // Task manager for the logged-in user
    TaskManager taskManager(taskFilename, username);

    // Display task management options
    int choice;
    do {
        cout << "\nTask Manager\n";
        cout << "1. Add Task\n";
        cout << "2. Delete Task\n";
        cout << "3. Complete Task\n";
        cout << "4. Display Tasks\n";
        cout << "5. Close Program\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string description, deadline;
                cout << "Enter task description: ";
                cin.ignore();
                getline(cin, description);
                cout << "Enter task deadline (YYYY-MM-DD): ";
                cin >> deadline;
                taskManager.addTask(description, deadline);
                break;
            }
            case 2: {
                int taskId;
                cout << "Enter task ID to delete: ";
                cin >> taskId;
                taskManager.deleteTask(taskId);
                break;
            }
            case 3: {
                int taskId;
                cout << "Enter task ID to mark as completed: ";
                cin >> taskId;
                taskManager.completeTask(taskId);
                break;
            }
            case 4:
                taskManager.displayTasks();
                break;
            case 5:
                cout << "Closing program." << endl;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 5);

    // Write users back to file
    writeUsersToFile(userFilename, users);

    return 0;
}
