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

void save_load::load(std::string file, std::string id, std::string name, Eigen::Tensor<double, 4>* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    int size = std::stoi(split_string_in_strings(line, 0x20)[1]);

    Eigen::Tensor<double, 4> returnvalue(size, size, size, size);

    std::getline(iss, line);
    std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);

    int idx = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                for (int l = 0; l < size; l++)
                {
                    returnvalue(i,j,k,l) = std::stod(linevalues[idx]);
                    idx++;
                }
            }
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

void save_load::load(std::string file, std::string id, std::string name, Eigen::MatrixXcd* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    int rows = std::stoi(split_string_in_strings(line, 0x20)[1]);
    std::getline(iss, line);
    int cols = std::stoi(split_string_in_strings(line, 0x20)[1]);

    Eigen::MatrixXcd returnvalue = Eigen::MatrixXcd::Zero(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        std::getline(iss, line);
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        for (int j = 0; j < cols; j++)
        {
            std::vector<std::string> complex_number = split_string_in_strings(linevalues[j], 0x5F);
            returnvalue(i, j).real(std::stod(complex_number[0]));
            returnvalue(i, j).imag(std::stod(complex_number[1]));
        }
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

void save_load::load(std::string file, std::string id, std::string name, std::vector<QC::Potential_Gaussian> *object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;
    while (std::getline(iss, line))
    {
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        if (linevalues.size() == 5)
        {
            QC::Potential_Gaussian new_Gausspotential;

            new_Gausspotential.set_center(std::stod(linevalues[0]), std::stod(linevalues[1]), std::stod(linevalues[2]));
            new_Gausspotential.set_coefficient(std::stod(linevalues[3]));
            new_Gausspotential.set_exponent(std::stod(linevalues[4]));
            object->push_back(new_Gausspotential);
        }
    }
}

void save_load::load(std::string file, std::string id, std::string name, QC::TDCI_WF* object)
{
    std::string filecontent = read_single_file(file, id, name);
    std::istringstream iss(filecontent);
    std::string line;

    std::getline(iss, line);
    int initial_states = std::stoi(split_string_in_strings(line, 0x20)[1]);
    std::getline(iss, line);
    std::vector<std::string> initial_states_list = split_string_in_strings(line, 0x20);
    for (int i = 0; i < initial_states; i++) { object->add_state(std::stoi(initial_states_list[i])); }

    std::getline(iss, line);

    std::getline(iss, line);
    int time_steps = std::stoi(split_string_in_strings(line, 0x20)[1]);
    std::getline(iss, line);
    std::vector<std::string> time_steps_list = split_string_in_strings(line, 0x20);
    for (int i = 0; i < time_steps; i++) { object->add_time(std::stod(time_steps_list[i])); }

    std::getline(iss, line);

    std::getline(iss, line);
    int total_states = std::stoi(split_string_in_strings(line, 0x20)[1]);

    std::getline(iss, line);

    for (int timestep = 0; timestep < time_steps; timestep++)
    {
        Eigen::MatrixXcd ci_coeff_at_time(total_states, initial_states);

        for (int i = 0; i < total_states; i++)
        {
            std::getline(iss, line);
            std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
            for (int j = 0; j < initial_states; j++)
            {
                std::vector<std::string> complex_number = split_string_in_strings(linevalues[j], 0x5F);
                ci_coeff_at_time(i, j).real(std::stod(complex_number[0]));
                ci_coeff_at_time(i, j).imag(std::stod(complex_number[1]));
            }
        }

        object->add_ci_vectors(ci_coeff_at_time);
        std::getline(iss, line);
    }
}

/*
void save_load::load(std::string id, std::string name, QC::Pauli_Operator* object)
{
    std::string filecontent = read_single_file(id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    object->set_number_qubits(std::stoi(split_string_in_strings(line, 0x20)[1]));
    std::getline(iss, line);
    object->set_Identity(std::stod(split_string_in_strings(line, 0x20)[1]));

    while (std::getline(iss, line))
    {
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        QC::Pauli_String PauliString;
        PauliString.set_number_qubits(std::stoi(linevalues[0]));
        PauliString.set_coefficient(std::stod(linevalues[1]));
        std::vector<std::string> pauli_operations;
        std::vector<int> correspong_qubits;

        std::vector<std::string> operations_and_qubits = split_string_in_strings(linevalues[2], 0x5F);
        for (int i = 0; i < operations_and_qubits.size(); i++)
        {
            pauli_operations.push_back(operations_and_qubits[i].substr(0, 1));
            correspong_qubits.push_back(std::stoi(operations_and_qubits[i].substr(1, operations_and_qubits[i].size() - 1)));
        }
        PauliString.set_PauliOperations(pauli_operations);
        PauliString.set_corresponding_Qubits(correspong_qubits);
        object->add_pauli_string(PauliString);
    }
}

void save_load::load(std::string id, std::string name, QC::QC_Statevector* object)
{
    std::string filecontent = read_single_file(id, name);
    std::istringstream iss(filecontent);
    std::string line;
    std::getline(iss, line);
    int number_amplitudes = std::stoi(split_string_in_strings(line, 0x20)[1]);

    for (int i = 0; i < number_amplitudes; i++)
    {
        std::getline(iss, line);
        std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
        object->add_amplitude(std::stoi(linevalues[0]), std::stod(linevalues[1]), std::stod(linevalues[2]));
    }
}

void save_load::load(std::string id, std::string name, QC::QC_TD_Statevectors* object)
{
    std::string filecontent = read_single_file(id, name);
    std::istringstream iss(filecontent);
    std::string line;

    std::vector<double> times;
    std::getline(iss, line);
    int number_timesteps = std::stoi(split_string_in_strings(line, 0x20)[1]);
    std::getline(iss, line);
    std::vector<std::string> time_steps_list = split_string_in_strings(line, 0x20);
    for (int i = 0; i < number_timesteps; i++) { times.push_back(std::stod(time_steps_list[i])); }

    std::getline(iss, line); //Empty Line

    for (int timestep = 0; timestep < times.size(); timestep++)
    {
        std::getline(iss, line);
        int number_amplitudes = std::stoi(split_string_in_strings(line, 0x20)[1]);
        QC::QC_Statevector statevec;
        for (int i = 0; i < number_amplitudes; i++)
        {
            std::getline(iss, line);
            std::vector<std::string> linevalues = split_string_in_strings(line, 0x20);
            statevec.add_amplitude(std::stoi(linevalues[0]), std::stod(linevalues[1]), std::stod(linevalues[2]));
        }
        object->add_statevector(times[timestep], statevec);
        std::getline(iss, line);
    }
}

void save_load::load(std::string id, std::string name, QC::QC_TD_Norm* object)
{
    std::string filecontent = read_single_file(id, name);
    std::istringstream iss(filecontent);
    std::string line;

    std::vector<double> times;
    std::getline(iss, line);
    int number_timesteps = std::stoi(split_string_in_strings(line, 0x20)[1]);

    for (int i = 0; i < number_timesteps; i++) {
        std::getline(iss, line);
        std::vector<std::string> time_steps_list = split_string_in_strings(line, 0x20);

        object->add_norm(std::stod(time_steps_list[0]), std::stod(time_steps_list[1]));
    }
}
*/










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

void save_load::save(std::string file, std::string id, std::string name, Eigen::Tensor<double, 4> data)
{
    std::ostringstream outStream;
    int indices = data.dimension(0);
    outStream << "size: " << indices << std::endl;
    for (int i = 0; i < indices; i++) {
        for (int j = 0; j < indices; j++) {
            for (int k = 0; k < indices; k++) {
                for (int l = 0; l < indices; l++) {
                    outStream << data(i, j, k, l) << " ";
                }
            }
        }
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

void save_load::save(std::string file, std::string id, std::string name, Eigen::MatrixXcd data)
{
    std::ostringstream outStream;
    outStream << "rows: " << data.rows() << std::endl;
    outStream << "cols: " << data.cols() << std::endl;
    for (int i = 0; i < data.rows(); i++)
    {
        for (int j = 0; j < data.cols(); j++)
        {
            outStream << data(i, j).real() << "_" << data(i, j).imag() << " ";
        }
        outStream << std::endl;
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

void save_load::save(std::string file, std::string id, std::string name, std::vector<QC::Potential_Gaussian> data)
{
    std::ostringstream outStream;
    for (int i = 0; i < data.size(); i++)
    {
        outStream << data[i].get_center_x() << " " << data[i].get_center_y() << " " << data[i].get_center_z() << " " << data[i].get_coefficient() << " " << data[i].get_exponent() << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}

void save_load::save(std::string file, std::string id, std::string name, QC::TDCI_WF data)
{
    std::ostringstream outStream;

    std::vector<int> state_data = data.get_states();
    std::vector<double> time_data = data.get_times();
    std::vector<Eigen::MatrixXcd> td_ci_vectros_data = data.get_ci_vectors();

    outStream << "Initial_States: " << state_data.size() << std::endl;
    for (int i = 0; i < state_data.size(); i++)
    {
        outStream << state_data[i] << " ";
    }
    outStream << std::endl << std::endl;

    outStream << "Timesteps: " << time_data.size() << std::endl;
    for (int i = 0; i < time_data.size(); i++)
    {
        outStream << time_data[i] << " ";
    }
    outStream << std::endl << std::endl;

    outStream << "Total_States: " << td_ci_vectros_data[0].rows() << std::endl << std::endl;

    for (int timestep = 0; timestep < td_ci_vectros_data.size(); timestep++)
    {
        Eigen::MatrixXcd ci_vectors = td_ci_vectros_data[timestep];

        for (int i = 0; i < ci_vectors.rows(); i++)
        {
            for (int j = 0; j < ci_vectors.cols(); j++)
            {
                outStream << ci_vectors(i, j).real() << "_" << ci_vectors(i, j).imag() << " ";
            }
            outStream << std::endl;
        }
        outStream << std::endl;
    }
    write_single_file(file, id, name, outStream.str());
}

/*
void save_load::save(std::string id, std::string name, QC::Pauli_Operator data)
{
    std::ostringstream outStream;
    outStream << "Qubits: " << data.get_number_qubits() << std::endl;
    outStream << "Identity: " << data.get_Identity() << std::endl;

    std::vector<QC::Pauli_String> Pauli_Strings = data.get_pauli_strings();
    for (int i = 0; i < Pauli_Strings.size(); i++)
    {
        QC::Pauli_String PauliString = Pauli_Strings[i];
        std::vector<std::string> PauliOperations = PauliString.get_PauliOperations();
        std::vector<int> corresponding_Qubits = PauliString.get_corresponding_Qubits();

        outStream << PauliString.get_number_qubits() << " " << PauliString.get_coefficient() << " ";
        for (int j = 0; j < PauliOperations.size(); j++)
        {
            outStream << PauliOperations[j] << corresponding_Qubits[j];
            if (j < PauliOperations.size() - 1) { outStream << "_"; }
        }
        outStream << std::endl;
    }
    write_single_file(id, name, outStream.str());
}

void save_load::save(std::string id, std::string name, QC::QC_Statevector data)
{
    std::ostringstream outStream;
    std::vector<QC::QC_Amplitude> Amplitudes = data.get_amplitudes();
    outStream << "Amplitudes: " << Amplitudes.size() << std::endl;

    for (int i = 0; i < Amplitudes.size(); i++)
    {
        outStream << Amplitudes[i].statenumber << " " << Amplitudes[i].real_amp << " " << Amplitudes[i].imag_amp << std::endl;
    }
    write_single_file(id, name, outStream.str());
}

void save_load::save(std::string id, std::string name, QC::QC_TD_Statevectors data)
{
    std::ostringstream outStream;
    std::vector<double> times = data.get_times();
    std::vector<QC::QC_Statevector> Statevectors = data.get_td_statevector();

    outStream << "Timesteps: " << times.size() << std::endl;
    for (int i = 0; i < times.size(); i++)
    {
        outStream << times[i] << " ";
    }
    outStream << std::endl << std::endl;

    for (int timestep = 0; timestep < times.size(); timestep++)
    {
        std::vector<QC::QC_Amplitude> Amplitudes = Statevectors[timestep].get_amplitudes();
        outStream << "Amplitudes: " << Amplitudes.size() << std::endl;
        for (int i = 0; i < Amplitudes.size(); i++)
        {
            outStream << Amplitudes[i].statenumber << " " << Amplitudes[i].real_amp << " " << Amplitudes[i].imag_amp << std::endl;
        }
        outStream << std::endl;
    }

    write_single_file(id, name, outStream.str());
}

void save_load::save(std::string id, std::string name, QC::QC_TD_Norm data)
{
    std::ostringstream outStream;
    std::vector<double> times = data.get_times();
    std::vector<double> norms = data.get_td_norm();

    outStream << "Timesteps: " << times.size() << std::endl;
    for (int i = 0; i < times.size(); i++)
    {
        outStream << times[i] << " " << norms[i] << std::endl;
    }


    write_single_file(id, name, outStream.str());
}
*/