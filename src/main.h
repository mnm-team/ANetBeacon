/**
 * @file main.h
 * @author Dominik Bitzer
 * @date 2017
 * @brief Main function and help function.
 * 
 */

#ifndef MAIN_H
#define MAIN_H
#include "openssl_sign.h"
#include "sender.h"
#include "rawsocket_LAN_Beacon.h"
#include "receiver.h"
#include "define.h"


/**
 * @brief Separates receiver from sender mode and has the main program logic. 
 * 
 * @return Success or failure code.
 * 
 */
int main(int argc, char **argv);

/**
 * @brief Help function, executed if unknown parameters have been received or user specifically asks for help. 
 * 
 */
void printHelp();

#endif
