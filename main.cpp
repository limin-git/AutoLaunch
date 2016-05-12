// AutoLaunch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


std::vector<std::string> get_process_list()
{
    std::vector<std::string> process_list;
    HANDLE process_snap = NULL;
    PROCESSENTRY32 process_entry_32 = { 0 };
    process_snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if ( INVALID_HANDLE_VALUE != process_snap )
    {
        process_entry_32.dwSize = sizeof(PROCESSENTRY32);

        if ( Process32First( process_snap, &process_entry_32 ) )
        {
            do
            {
                std::string exec_file = process_entry_32.szExeFile;
                std::transform( exec_file.begin(), exec_file.end(), exec_file.begin(), ::tolower );
                process_list.push_back( exec_file );
            }
            while ( Process32Next( process_snap, &process_entry_32 ) );
        }
    }

    CloseHandle(process_snap);
    return process_list;
}


int _tmain(int argc, _TCHAR* argv[])
{
    std::string process_name;
    size_t wait_time = 0;
    std::string batch_file;
    std::string debug_file;
    size_t refresh_interval = 0;

    boost::program_options::variables_map vm;
    boost::program_options::options_description desc( "Options", 100 );
    desc.add_options()
        ( "process-name,P", boost::program_options::value<std::string>(), "process name" )
        ( "wait-time,W", boost::program_options::value<size_t>()->default_value(0), "wait time (in milliseconds)" )
        ( "batch-file,B", boost::program_options::value<std::string>(), "batch file" )
        ( "debug-file,D", boost::program_options::value<std::string>()->default_value( "AutoLaunch.log" ), "debug file" )
        ( "refresh-interval,R", boost::program_options::value<size_t>()->default_value( 500 ), "refresh interval" )
        ;

    boost::program_options::store( boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm );
    boost::program_options::notify(vm);

    debug_file = vm["debug-file"].as<std::string>();
    std::ofstream LOG( debug_file.c_str() );

    if ( 1 == argc )
    {
        LOG << std::endl << desc << std::endl;
        return 0;
    }

    if ( !vm.count( "process-name" ) )
    {
        LOG << "error: process name is empty.\n\n" << desc << std::endl;
        return 0;
    }

    if ( !vm.count( "batch-file" ) )
    {
        LOG << "error: batch file is empty.\n\n" << desc << std::endl;
        return 0;
    }

    process_name = vm["process-name"].as<std::string>();
    wait_time = vm["wait-time"].as<size_t>();
    batch_file = vm["batch-file"].as<std::string>();
    refresh_interval = vm["refresh-interval"].as<size_t>();

    LOG
        << "process-name:\t\t" << process_name << "\n"
        << "wait-time:\t\t" << wait_time << "\n"
        << "batch-file:\t\t" << batch_file << "\n"
        << "debug-file:\t\t" << debug_file << "\n"
        << "refresh-interval:\t" << refresh_interval << std::endl;

    if ( process_name.empty() )
    {
        LOG << "error: process name is empty." << std::endl;
        return 0;
    }

    if ( batch_file.empty() )
    {
        LOG << "error: batch file is empty." << std::endl;
        return 0;
    }

    if ( ! boost::filesystem::exists( batch_file ) )
    {
        LOG << "error: batch file \'"<< batch_file << "\' does not exist." << std::endl;
        return 0;
    }

    std::transform( process_name.begin(), process_name.end(), process_name.begin(), ::tolower );

    if ( std::string::npos == process_name.find( ".exe" ) )
    {
        process_name += ".exe";
    }

    LOG << "finding process: " << process_name << std::endl;

    while ( true )
    {
        std::vector<std::string> processes = get_process_list();

        if ( std::find( processes.begin(), processes.end(), process_name ) != processes.end() )
        {
            LOG << std::endl;
            std::copy( processes.begin(), processes.end(), std::ostream_iterator<std::string>( LOG, "\n" ) );
            break;
        }

        LOG << "." << std::flush;
        Sleep( refresh_interval );
    }

    LOG << "waiting for " << wait_time << " ms" << std::endl;
    Sleep( wait_time );

    std::string cmd = "START CMD /C " + batch_file;
    LOG << cmd << std::endl;
    system( cmd.c_str() );  
    LOG << "DONE" << std::endl;

	return 0;
}
