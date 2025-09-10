#include "logger.h"
#include <iostream>

int main() {
    Logger logger;
    
    // Initialize logger
    logger.init("ace", "./", 2, 10, "./logbackup/", 100);
    
    // Test logging
    logger.logMsg("Test message 1");
    logger.logMsg("Test message 2");
    logger.logFact("Force output message");
    
    // Test terminal disable
    logger.disableTerminal();
    logger.logMsg("This should not appear in terminal");
    
    // Test backup
    logger.backup();
    
    std::cout << "Logger test completed successfully!" << std::endl;
    return 0;
}
