// Ctrl+C harness for TC-Sim-002-N (TMS-F-061).
//
// Starts a program with its stdout redirected to a file, waits <delay_ms>, then
// delivers a genuine user interrupt: CTRL_C_EVENT on Windows (in a private
// console so only the harness and the child receive it) or SIGINT on POSIX.
// Exits with the child's exit code.
//
// Usage: send_ctrl_c <delay_ms> <stdout_file> <program> [args...]

#include <cstdio>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::fprintf(stderr, "usage: %s <delay_ms> <stdout_file> <program> [args...]\n", argv[0]);
        return 2;
    }
    const DWORD delay = static_cast<DWORD>(std::strtoul(argv[1], nullptr, 10));

    std::string commandLine;
    for (int i = 3; i < argc; ++i) {
        if (i > 3) commandLine += ' ';
        commandLine += '"';
        commandLine += argv[i];
        commandLine += '"';
    }

    // Private, hidden console: CTRL_C_EVENT reaches only processes attached to it.
    FreeConsole();
    if (!AllocConsole()) {
        std::fprintf(stderr, "AllocConsole failed (%lu)\n", GetLastError());
        return 2;
    }
    if (HWND window = GetConsoleWindow()) ShowWindow(window, SW_HIDE);
    // A parent shell may have started us with Ctrl+C disabled; that flag is
    // inherited, so re-enable it before creating the child.
    SetConsoleCtrlHandler(nullptr, FALSE);

    SECURITY_ATTRIBUTES inherit{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    HANDLE output = CreateFileA(argv[2], GENERIC_WRITE, FILE_SHARE_READ, &inherit, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, nullptr);
    if (output == INVALID_HANDLE_VALUE) return 2;

    STARTUPINFOA startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = output;
    startup.hStdError = output;
    PROCESS_INFORMATION process{};
    // The child must share this console and must not be in a new process group,
    // otherwise Windows would not deliver CTRL_C_EVENT to it.
    if (!CreateProcessA(nullptr, &commandLine[0], nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startup, &process)) {
        std::fprintf(stderr, "CreateProcess failed (%lu)\n", GetLastError());
        return 2;
    }

    Sleep(delay);
    SetConsoleCtrlHandler(nullptr, TRUE);  // the harness itself ignores the interrupt
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);

    if (WaitForSingleObject(process.hProcess, 10000) == WAIT_TIMEOUT) {
        TerminateProcess(process.hProcess, 124);
        std::fprintf(stderr, "child did not stop within 10 s of Ctrl+C\n");
    }
    DWORD exitCode = 1;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    CloseHandle(output);
    return static_cast<int>(exitCode);
}

#else
#include <csignal>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::fprintf(stderr, "usage: %s <delay_ms> <stdout_file> <program> [args...]\n", argv[0]);
        return 2;
    }
    const unsigned delay = static_cast<unsigned>(std::strtoul(argv[1], nullptr, 10));
    const pid_t child = fork();
    if (child < 0) return 2;
    if (child == 0) {
        const int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) _exit(2);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        execv(argv[3], &argv[3]);
        _exit(127);
    }
    usleep(delay * 1000u);
    kill(child, SIGINT);
    int status = 0;
    waitpid(child, &status, 0);
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return 128 + (WIFSIGNALED(status) ? WTERMSIG(status) : 0);
}
#endif
