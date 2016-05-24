#ifndef GARNETD__H__
#define GARNETD__H__

#define _CRT_SECURE_NO_WARNINGS

#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <GetTime.h>
#include <Garnet.h>
#include <Exception.h>

const enum {
    MODE_EVAL_COMMAND,
    MODE_EVAL_BATCH,
    MODE_GARNET_BATCH,
    MODE_GARNET_DAEMON,
};

struct GarnetDConf
{
    bool  outputCSV;
    bool  outputImage;
    bool  outputHTML;
    bool  verbose;
    int   mode;
    std::string  workingDir;
    int   initialGen;

    std::map<int, std::string> srcs;
    std::vector<std::string>  aliasDics;
    std::vector<std::string>  trees;
    std::string  project;
    int  height;
    int  width;
};

int  execute_eval_command(const GarnetDConf& conf);
int  execute_eval_batch(const GarnetDConf& conf);
int  execute_garnet_batch(const GarnetDConf& conf);
int  execute_garnet_daemon(const GarnetDConf& conf);


void  makeDir(const std::string& path);
bool  existsDir(const std::string& path);
bool  existsFile(const std::string& path);


#endif//GARNETD__H__
