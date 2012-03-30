#include "drmaa2.h"
#include "drmaa2-list.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Constants, filled on demand in according functions
drmaa2_version version=NULL;

// TODO:    use jobinfo
//          replace pid by id
typedef struct drmaa2_j_s
{
    const char *id;
    const char *session_name;
    pid_t pid;
} drmaa2_j_s;

typedef struct drmaa2_jsession_s
{
    const char *contact;
    const char *name;
    drmaa2_j_h jobs;
} drmaa2_jsession_s;

drmaa2_jtemplate  drmaa2_jtemplate_create(void)
{
    drmaa2_jtemplate jt = (drmaa2_jtemplate)malloc(sizeof(drmaa2_jtemplate_s));
    jt->remoteCommand       = DRMAA2_UNSET_STRING;
    jt->args                = DRMAA2_UNSET_LIST;          
    jt->submitAsHold        = DRMAA2_UNSET_BOOL;
    jt->rerunnable          = DRMAA2_UNSET_BOOL;
    jt->jobEnvironment      = DRMAA2_UNSET_DICT;
    jt->workingDirectory    = DRMAA2_UNSET_STRING;        
    jt->jobCategory         = DRMAA2_UNSET_STRING;            
    jt->email               = DRMAA2_UNSET_LIST;
    jt->emailOnStarted      = DRMAA2_UNSET_BOOL;
    jt->emailOnTerminated   = DRMAA2_UNSET_BOOL;
    jt->jobName             = DRMAA2_UNSET_STRING;              
    jt->inputPath           = DRMAA2_UNSET_STRING;
    jt->outputPath          = DRMAA2_UNSET_STRING;
    jt->errorPath           = DRMAA2_UNSET_STRING;
    jt->joinFiles           = DRMAA2_UNSET_BOOL;
    jt->reservationId       = DRMAA2_UNSET_STRING;
    jt->queueName           = DRMAA2_UNSET_STRING;
    jt->minSlots            = DRMAA2_UNSET_NUM;
    jt->maxSlots            = DRMAA2_UNSET_NUM;
    jt->priority            = DRMAA2_UNSET_NUM;
    jt->candidateMachines   = DRMAA2_UNSET_LIST;
    jt->minPhysMemory       = DRMAA2_UNSET_NUM;
    jt->machineOS           = DRMAA2_UNSET_ENUM;
    jt->machineArch         = DRMAA2_UNSET_ENUM;
    jt->startTime           = DRMAA2_UNSET_TIME;
    jt->deadlineTime        = DRMAA2_UNSET_TIME;
    jt->stageInFiles        = DRMAA2_UNSET_DICT; 
    jt->stageOutFiles       = DRMAA2_UNSET_DICT;
    jt->resourceLimits      = DRMAA2_UNSET_DICT;
    jt->accountingId        = DRMAA2_UNSET_STRING;
    return jt;
}


drmaa2_error drmaa2_jtemplate_free(drmaa2_jtemplate jt)
{
    free(jt);
    return 0;
}


drmaa2_j_h drmaa2_jsession_run_job(const drmaa2_jsession_h js, const drmaa2_jtemplate jt)
{
    pid_t childpid;
    char ** args;

    //TODO: copy job template, work only with the copy 
    if ((childpid = fork()) == -1)
    {
        perror("fork failed\n");
        exit(1);
    }
    else if (childpid == 0)
        {
            // child
            if (jt->args) 
		args=stringlist_get_array(jt->args);
            execv(jt->remoteCommand, args);
            if (jt->args) stringlist_free_array(args);
            exit(0);
        }
        else
        {
            // parent
            drmaa2_j_h j = (drmaa2_j_h)malloc(sizeof(drmaa2_j_s));
            j->session_name = js->name;
            j->pid = childpid;
            return j;
        }
}


drmaa2_j_h drmaa2_j_wait_terminated(const drmaa2_j_h j, const time_t timeout)
{
    pid_t child;
    int status;

    child = waitpid(j->pid, &status, 0);
    return j;
}


char * drmaa2_get_drms_name(void)
{
    return NULL;
}

drmaa2_version drmaa2_get_drms_version(void)
{
    return NULL;
}

char *drmaa2_get_drmaa_name(void)
{
    return "drmaa2-mock";
}

drmaa2_version drmaa2_get_drmaa_version(void)
{
    if (version == NULL) {
        version=malloc(sizeof(drmaa2_version_s));
        version->major="2";
        version->minor="0";
    }
    return version;
}


drmaa2_jsession_h drmaa2_create_jsession(const char * session_name, const char * contact){
    drmaa2_jsession_h js = (drmaa2_jsession_h)malloc(sizeof(drmaa2_jsession_s));
    js->name = session_name;
    js->contact = contact;
    // TODO: append job-session to js-list
    return js;
}


drmaa2_error drmaa2_close_jsession(drmaa2_jsession_h js)
{
    // should this method be called before destruction?
    free(js);
    // TODO: persist information (which??)
    return 0;
}


drmaa2_error drmaa2_destroy_jsession(const char * session_name)
{
    // TODO: reap persistent information
    return 0;
}
