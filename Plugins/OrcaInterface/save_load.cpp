#include "save_load.h"

std::string save_load::read_single_file(std::string file, std::string id, std::string name)
{
    std::string zip_filename = file;
    std::string content_filename = id + "_" + name;

    //Open zip file
    int error(0);
    zip* zipfile = zip_open(zip_filename.c_str(), 0, &error);

    //Search for the file of given name
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(zipfile, content_filename.c_str(), 0, &st);

    //Alloc memory for its uncompressed contents
    char* contents = new char[st.size];

    //Read the compressed file
    zip_file* f = zip_fopen(zipfile, content_filename.c_str(), 0);
    zip_fread(f, contents, st.size);
    zip_fclose(f);

    //And close the archive
    zip_close(zipfile);

    return std::string(contents).substr(0, st.size);
}

void save_load::write_single_file(std::string file, std::string id, std::string name, std::string content)
{
    std::string zip_filename = file;
    std::string content_filename = id + "_" + name;

    int error(0);
    zip_t* zipfile = zip_open(zip_filename.c_str(), 0, &error);
    zip_source* source = zip_source_buffer(zipfile, content.c_str(), content.size(), 0);
    zip_file_add(zipfile, content_filename.c_str(), source, ZIP_FL_OVERWRITE);
    zip_close(zipfile);
}

std::vector<std::string> save_load::split_string_in_strings(std::string string, char delimiter)
{
    std::vector<std::string> return_vec;
    std::istringstream iss(string);
    std::string s;
    while (std::getline(iss, s, delimiter)) {
        return_vec.push_back(s);
    }
    return return_vec;
}










void save_load::load(std::string file, std::string id, std::string name, int* object)
{
    std::string content = read_single_file(file, id, name);
    *object = std::stoi(content.c_str());
}

void save_load::load(std::string file, std::string id, std::string name, double* object)
{
    std::string content = read_single_file(file, id, name);
    *object = std::stod(content.c_str());
}

void save_load::load(std::string file, std::string id, std::string name, std::vector<QC::Pointcharge>* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    while (std::getline(iss, line))
    {
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        if (linevalues.size() == 4)
        {
            QC::Pointcharge new_Pointcharge;
            new_Pointcharge.set_center(std::stod(linevalues[0]), std::stod(linevalues[1]), std::stod(linevalues[2]));
            new_Pointcharge.set_charge(std::stoi(linevalues[3]));
            object->push_back(new_Pointcharge);
        }
    }
}

void save_load::load(std::string file, std::string id, std::string name, QC::Gaussian_Basisset* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    object->set_basisset_type(std::stoi(split_string_in_strings(line, 0x20)[1]));
    while (std::getline(iss, line))
    {
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        if (linevalues.size() > 0)
        {
            QC::Gaussian_Basisfunction new_Basisfunction;
            new_Basisfunction.set_center(std::stod(linevalues[0]), std::stod(linevalues[1]), std::stod(linevalues[2]));
            new_Basisfunction.set_total_angular_moment(std::stoi(linevalues[3]));
            int contractionlevel = std::stoi(linevalues[4]);
            for (int i = 0; i < contractionlevel; i++)
            {
                new_Basisfunction.add_parameterdouble(std::stod(linevalues[5 + i * 2]), std::stod(linevalues[6 + i * 2]));
            }
            object->add_Basisfunction(new_Basisfunction);
        }
    }
}

void save_load::load(std::string file, std::string id, std::string name, Eigen::MatrixXd* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    int rows = std::stoi(split_string_in_strings(line, 0x20)[1]);
    std::getline(iss, line);
    int cols = std::stoi(split_string_in_strings(line, 0x20)[1]);

    Eigen::MatrixXd	 returnvalue = Eigen::MatrixXd::Zero(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        std::getline(iss, line);
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        for (int j = 0; j < cols; j++)
        {
            returnvalue(i, j) = std::stod(linevalues[j]);
        }
    }
    *object = returnvalue;
}

void save_load::load(std::string file, std::string id, std::string name, Eigen::VectorXd* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    int rows = std::stoi(split_string_in_strings(line, 0x20)[1]);

    Eigen::VectorXd	 returnvalue = Eigen::VectorXd::Zero(rows);

    for (int i = 0; i < rows; i++)
    {
        std::getline(iss, line);
        returnvalue(i) = std::stod(line);
    }

    *object = returnvalue;
}

void save_load::load(std::string file, std::string id, std::string name, std::vector<std::string>* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    while (std::getline(iss, line))
    {
        if (line != "")
        {
            object->push_back(line);
        }
    }
}







void save_load::save(std::string file, std::string id, std::string name, int data)
{
    std::ostringstream outStream;
    outStream << data;
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, double data)
{
    std::ostringstream outStream;
    outStream << data;
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, std::vector<QC::Pointcharge> data)
{
    std::ostringstream outStream;
    for (int i = 0; i < data.size(); i++)
    {
        outStream << data[i].get_center_x() << " " << data[i].get_center_y() << " " << data[i].get_center_z() << " " << data[i].get_charge() << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, QC::Gaussian_Basisset data)
{
    std::ostringstream outStream;
    outStream << "Basissettype: " << data.get_basisset_type() << std::endl;
    for (int i = 0; i < data.get_Basisfunctionnumber_angular_compact(); i++)
    {
        QC::Gaussian_Basisfunction Basisfunction = data.get_Basisfunction(i);
        outStream << Basisfunction.get_center_x() << " " << Basisfunction.get_center_y() << " " << Basisfunction.get_center_z() << " " << Basisfunction.get_total_angular_moment() << " " << Basisfunction.get_contraction();
        for (int j = 0; j < Basisfunction.get_contraction(); j++)
        {
            outStream << " " << Basisfunction.get_coefficient(j) << " " << Basisfunction.get_exponent(j);
        }
        outStream << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, Eigen::MatrixXd data)
{
    std::ostringstream outStream;
    outStream << "rows: " << data.rows() << std::endl;
    outStream << "cols: " << data.cols() << std::endl;
    for (int i = 0; i < data.rows(); i++)
    {
        for (int j = 0; j < data.cols(); j++)
        {
            outStream << data(i, j) << " ";
        }
        outStream << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, Eigen::VectorXd data)
{
    std::ostringstream outStream;
    outStream << "rows: " << data.rows() << std::endl;
    for (int i = 0; i < data.rows(); i++)
    {
        outStream << data(i) << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, std::vector<std::string> data)
{
    std::ostringstream outStream;
    for (int i = 0; i < data.size(); i++)
    {
        outStream << data[i] << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}