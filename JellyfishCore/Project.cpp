#include "Project.h"
#include "Core.hpp"
#include <zip.h>
#include <zipconf.h>
    
Project::Project(int Projectnumber, CoreSystem* Core, std::string Projectname)
{
    _ProjectNumber = Projectnumber; 
    _Core = Core; 
    _ProjectName = Projectname;
}

Project::Project(int Projectnumber, CoreSystem* Core, std::string Projectname, std::string Projectfile, bool PrintConsoleOutput)
{
    _ProjectNumber = Projectnumber; 
    _Core = Core; 
    _ProjectName = Projectname; 
    load(Projectfile, PrintConsoleOutput);
}

void Project::add_node(std::shared_ptr<Node> node)
{
    all_nodes[node->id] = node;
}
void Project::remove_node(std::string nodeID)
{
    all_nodes.erase(nodeID);
}

void Project::add_connection(std::shared_ptr<Connection> connection)
{
    all_connections[connection->_Id] = connection;
    connection->_InNode->add_connection(true, connection);
    connection->_OutNode->add_connection(false, connection);
}

void Project::load_connection(std::shared_ptr<Connection> connection) //like add_connection but does not reset the NodeState of dependent nodes
{
    all_connections[connection->_Id] = connection;
    connection->_InNode->load_connection(true, connection);
    connection->_OutNode->load_connection(false, connection);
    connection->_InNode->setInData(connection->_InPort, connection->_OutNode->getOutData(connection->_OutPort));
}

void Project::remove_connection(std::string connectionID)
{
    all_connections[connectionID]->_InNode->remove_connection(true, connectionID);
    all_connections[connectionID]->_OutNode->remove_connection(false, connectionID);
    all_connections.erase(connectionID);
}

void Project::save(std::string file, bool PrintConsoleOutput)
{
    if(!file.ends_with(".jlf"))
    {
        file = file + ".jlf";
    }

    std::vector<std::string> splitted = Utils::split_string_in_strings(file, '/');
    _ProjectName = splitted[splitted.size() - 1];
    size_t pos = _ProjectName.find(".jlf");
    if (pos != std::string::npos)
    {
        _ProjectName.erase(pos, 10);
    }

    addToLog("Project saved as: " + file);

    //Create an empty jellyfish file and add an empty modulefile
    {
        int error(0);
        remove(file.c_str());
        zip_t* zipfile = zip_open(file.c_str(), ZIP_CREATE | ZIP_EXCL, &error);
        zip_source* source = zip_source_buffer(zipfile, "", 0, 0);
        zip_file_add(zipfile, "main", source, ZIP_FL_OVERWRITE);
        zip_close(zipfile);
    }
    
    json mainfile;

    //Save Connections
    std::vector<json> connections;
    for (auto const& [key, val] : all_connections)
    {
        json con;
        con["ID"] = key;
        con["OutNode"] = val->_OutNode->id;
        con["OutPort"] = val->_OutPort;
        con["InNode"] = val->_InNode->id;
        con["InPort"] = val->_InPort;
        connections.push_back(con);
    }
    mainfile["Connections"] = connections;

    std::vector<json> nodes;
    for (auto const& [key, val] : all_nodes)
    {
        json node;
        node["ID"] = key;
        node["TypeID"] = val->NodeTypeID;
        node["Plugin"] = val->Plugin;
        node["x"] = val->x_position_on_grid;
        node["y"] = val->y_position_on_grid;
        if(val->State == NodeState::Calculated)
        {
            node["Calculated"] = true;
        }
        else
        {
            node["Calculated"] = false;
        }
        node["Data"] = val->SaveData(file);
        nodes.push_back(node);
        if(PrintConsoleOutput){ std::cout << "Node: " << val->NodeName << " with ID: " << val->id << " saved" << std::endl;}
    }
    mainfile["Nodes"] = nodes;

    mainfile["Log"] = _Log;

    std::string mainfilefilestring = mainfile.dump();

    //Save finished mainfile
    {
        int error(0);
        zip_t* zipfile = zip_open(file.c_str(), 0, &error);
        zip_source* source = zip_source_buffer(zipfile, mainfilefilestring.c_str(), mainfilefilestring.size(), 0);
        zip_file_add(zipfile, "main", source, ZIP_FL_OVERWRITE);
        zip_close(zipfile);
    }
    if(PrintConsoleOutput){ std::cout << "Project: " << get_ProjectName() << " saved" << std::endl;}
}

void Project::load(std::string file, bool PrintConsoleOutput)
{
    std::vector<std::string> splitted = Utils::split_string_in_strings(file, '/');
    _ProjectName = splitted[splitted.size() - 1];
    size_t pos = _ProjectName.find(".jlf");
    if (pos != std::string::npos)
    {
        _ProjectName.erase(pos, 10);
    }

    int error(0);
    zip* zipfile = zip_open(file.c_str(), 0, &error);
    if (error != 0)
    {
        std::cout << "File could not be read or is not available" << std::endl;
        return;
    }

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(zipfile, "main", 0, &st);

    //Alloc memory for its uncompressed contents
    char* contents = new char[st.size];

    //Read the compressed file
    zip_file* f = zip_fopen(zipfile, "main", 0);
    zip_fread(f, contents, st.size);
    zip_fclose(f);

    //Close the archive
    zip_close(zipfile);
    std::string temp = std::string(contents).substr(0, st.size);
    json mainfile = json::parse(temp);

    //std::cout << "TEMP: " << temp << std::endl;
    //std::cout << "JSON: " << mainfile << std::endl;
    //std::cout << "JSON: " << mainfile["Nodes"] << std::endl;

    for (int i = 0; i < mainfile["Nodes"].size(); i++)
    {
        json node = mainfile["Nodes"][i];
        std::shared_ptr<Node> n = _Core->CreateNode(node["Plugin"], node["TypeID"]);
        n->id = node["ID"];
        n->x_position_on_grid = node["x"];
        n->y_position_on_grid = node["y"];
        if(node["Calculated"] == true)
        {
            n->State = NodeState::Calculated;
        }
        else
        {
            n->State = NodeState::notCalculated;
        }
        n->LoadData(node["Data"], file);
        add_node(n);
        if(PrintConsoleOutput){ std::cout << "Node: " << n->NodeName << " with ID: " << n->id << " loaded" << std::endl;}
    }

    for (int i = 0; i < mainfile["Connections"].size(); i++)
    {
        json con = mainfile["Connections"][i];
        std::shared_ptr<Node> OutNode = all_nodes[con["OutNode"]];
        int OutPort = con["OutPort"];
        std::shared_ptr<Node> InNode = all_nodes[con["InNode"]];
        int InPort = con["InPort"];
        std::string Id = con["ID"];

        load_connection(std::make_shared<Connection>(OutNode, OutPort, InNode, InPort, Id));
    }

    _Log = mainfile["Log"].get<std::vector<std::string>>();
    if(PrintConsoleOutput){ std::cout << "Project: " << get_ProjectName() << " loaded" << std::endl;}
}

void Project::addToLog(std::string logdata)
{
    std::time_t t = std::time(nullptr);
    std::string time = std::ctime(&t);

    if (!time.empty() && time[time.length()-1] == '\n') {
        time.erase(time.length()-1);
    }

    std::string temp = "[" + time + "] " +  logdata;
    _Log.push_back(temp);
}

void Project::compute_node(std::string nodeID, bool PrintConsoleOutput)
{
    if (all_nodes.find(nodeID) == all_nodes.end()){ std::cout << "Node not found" << std::endl; return; }
    addToLog("Compute Node: " + nodeID);
    if(PrintConsoleOutput){ std::cout << "Compute Node: " << nodeID << " (" << all_nodes[nodeID]->NodeName << ")" << std::endl;}
    all_nodes[nodeID]->calculate();
}

void Project::compute_node_and_dependence(std::string nodeID, bool PrintConsoleOutput)
{
    for (auto const& [key, val] : all_connections)
    {
        std::string outID = val->_OutNode->id;
        std::string inID = val->_InNode->id;
        std::shared_ptr<Node> outNode = all_nodes[outID];
        
        if(inID == nodeID && outNode->HasCalulateFunction && outNode->State == NodeState::notCalculated)
        {
            compute_node_and_dependence(outID, PrintConsoleOutput);
        }
    }
    compute_node(nodeID, PrintConsoleOutput);
}