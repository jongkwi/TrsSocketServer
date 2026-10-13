/*
**	onlicmain.h
*/
#ifndef ___ONLIC_MAIN_H___
#define ___ONLIC_MAIN_H___

int main(int argc, char *argv[]);
int RunOnlicSystem();
int StopOnlicSystem();
int is_check_process(pid_t my_pid, const char *szPName);
void OnlicIntro();
void GetProcessName(const char *szFullPath, char *szProcName);
void GetCompileDefineString(char* szDef);

#endif /* onlicmain.h */
