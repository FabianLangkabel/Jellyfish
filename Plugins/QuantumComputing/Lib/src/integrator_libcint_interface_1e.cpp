#include <iostream>
#include "../header/integrator_libcint_interface_1e.h"

namespace QC
{
 
    void Integrator_libint_interface_1e::compute()
    {
        int number_atoms = this->Basisset.get_Basisfunctionnumber_angular_compact() + this->Pointcharges.size();
        int number_basisfunction = this->Basisset.get_Basisfunctionnumber_angular_compact();
        int number_basisfunction_angular_expanded = this->Basisset.get_Basisfunctionnumber_angular_expanded();

        // ATM_SLOTS = 6; BAS_SLOTS = 8;
        int* atoms = (int*)malloc(sizeof(int) * number_atoms * ATM_SLOTS);
        int* basisfunctions = (int*)malloc(sizeof(int) * number_basisfunction * BAS_SLOTS);
        double* enviroment = (double*)malloc(sizeof(double) * 10000);

        int offset = PTR_ENV_START; // = 20;


        for (int i = 0; i < number_basisfunction; i++)
        {
            atoms[CHARGE_OF + ATM_SLOTS * i] = 0;
            atoms[PTR_COORD + ATM_SLOTS * i] = offset;
            enviroment[offset + 0] = this->Basisset.get_Basisfunction(i).get_center_x(); // x (Bohr)
            enviroment[offset + 1] = this->Basisset.get_Basisfunction(i).get_center_y(); // y (Bohr)
            enviroment[offset + 2] = this->Basisset.get_Basisfunction(i).get_center_z(); // z (Bohr)
            offset += 3;
        }
        
        for (int j = 0; j < this->Pointcharges.size(); j++)
        {
            atoms[CHARGE_OF + ATM_SLOTS * (j + number_basisfunction)] = this->Pointcharges[j].get_charge();
            atoms[PTR_COORD + ATM_SLOTS * (j + number_basisfunction)] = offset;
            enviroment[offset + 0] = this->Pointcharges[j].get_center_x(); // x (Bohr)
            enviroment[offset + 1] = this->Pointcharges[j].get_center_y(); // y (Bohr)
            enviroment[offset + 2] = this->Pointcharges[j].get_center_z(); // z (Bohr)
            offset += 3;
        }


        for (int n = 0; n < number_basisfunction; n++)
        {
            basisfunctions[ATOM_OF + BAS_SLOTS * n] = n; //Corresponding Atom
            basisfunctions[ANG_OF + BAS_SLOTS * n] = this->Basisset.get_Basisfunction(n).get_total_angular_moment(); //Angular Moment
            basisfunctions[NPRIM_OF + BAS_SLOTS * n] = this->Basisset.get_Basisfunction(n).get_contraction(); //Number of primitiv GTO
            basisfunctions[NCTR_OF + BAS_SLOTS * n] = 1; //Number of contracted GTO
            basisfunctions[PTR_EXP + BAS_SLOTS * n] = offset; 
            for (int m = 0; m < this->Basisset.get_Basisfunction(n).get_contraction(); m++)
            {
                enviroment[offset + m] = this->Basisset.get_Basisfunction(n).get_exponent(m);
            }
            offset += this->Basisset.get_Basisfunction(n).get_contraction();
            basisfunctions[PTR_COEFF + BAS_SLOTS * n] = offset;
            for (int m = 0; m < this->Basisset.get_Basisfunction(n).get_contraction(); m++)
            {
                enviroment[offset + m] = this->Basisset.get_Basisfunction(n).get_coefficient(m) * CINTgto_norm(basisfunctions[ANG_OF + BAS_SLOTS * n], enviroment[basisfunctions[PTR_EXP + BAS_SLOTS * n] + m ]);
            }
            offset += this->Basisset.get_Basisfunction(n).get_contraction();
        }

        int i, j, k, l;
        int di, dj, dk, dl;
        int shls[4];
        double* buf;

        if(this->Integral_Type < 100)
        {
            this->Integrals = Eigen::MatrixXd::Zero(number_basisfunction_angular_expanded, number_basisfunction_angular_expanded);


            int block_start_i = 0;
            for (i = 0; i < number_basisfunction; i++)
            {
                shls[0] = i; di = CINTcgto_spheric(i, basisfunctions);
                int block_start_j = 0;
                for (j = 0; j < number_basisfunction; j++)
                {
                    shls[1] = j; dj = CINTcgto_spheric(j, basisfunctions);



                    if (this->Integral_Type == 0) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_ovlp_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_ovlp_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }

                        int element = 0;
                        for (int j2 = 0; j2 < dj; j2++) {
                            for (int i2 = 0; i2 < di; i2++) {

                                this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                element++;
                            }
                        }
                        free(buf);
                    }
                    if (this->Integral_Type == 1) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_kin_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_kin_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = 0;
                        for (int j2 = 0; j2 < dj; j2++) {
                            for (int i2 = 0; i2 < di; i2++) {

                                this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                element++;
                            }
                        }
                        free(buf);
                    }
                    if (this->Integral_Type == 2) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        enviroment[1] = 0;
                        enviroment[2] = 0;
                        enviroment[3] = 0;
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_r_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_r_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = 0;
                        for (int j2 = 0; j2 < dj; j2++) {
                            for (int i2 = 0; i2 < di; i2++) {

                                this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                element++;
                            }
                        }
                        free(buf);
                    }
                    if (this->Integral_Type == 3) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        enviroment[1] = 0;
                        enviroment[2] = 0;
                        enviroment[3] = 0;
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_r_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_r_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = di * dj;
                        for (int j2 = 0; j2 < dj; j2++) {
                            for (int i2 = 0; i2 < di; i2++) {

                                this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                element++;
                            }
                        }
                        free(buf);
                    }
                    if (this->Integral_Type == 4) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        enviroment[1] = 0;
                        enviroment[2] = 0;
                        enviroment[3] = 0;
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_r_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_r_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = 2 * di * dj;
                        for (int j2 = 0; j2 < dj; j2++) {
                            for (int i2 = 0; i2 < di; i2++) {
                                this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                element++;
                            }
                        }
                        free(buf);
                    }
                    if (this->Integral_Type == 5) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_nuc_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_nuc_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = 0;
                        for (int j2 = 0; j2 < dj; j2++) {
                            for (int i2 = 0; i2 < di; i2++) {

                                this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                element++;
                            }
                        }
                        free(buf);
                    }

                    if (this->Integral_Type == 7) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 9);
                        enviroment[1] = 0;
                        enviroment[2] = 0;
                        enviroment[3] = 0;
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_rr_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_rr_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = 0;
                        for (int component = 0; component < 9; component++) {
                            for (int j2 = 0; j2 < dj; j2++) {
                                for (int i2 = 0; i2 < di; i2++) {
                                    this->Integrals(block_start_i + i2, block_start_j + j2) = buf[element];
                                    element++;
                                }
                            }
                        }
                        free(buf);
                    }
                    block_start_j += dj;
                }
                block_start_i += di;
            }
        }
        else
        {
            multiple_Integrals.clear();
            int components;
            if (this->Integral_Type == 100) { components = 3; }
            if (this->Integral_Type == 101) { components = 9; }

            for (int i = 0; i < components; i++)
            {
                this->multiple_Integrals.push_back(Eigen::MatrixXd::Zero(number_basisfunction_angular_expanded, number_basisfunction_angular_expanded));
            }


            int block_start_i = 0;
            for (i = 0; i < number_basisfunction; i++)
            {
                shls[0] = i; di = CINTcgto_spheric(i, basisfunctions);
                int block_start_j = 0;
                for (j = 0; j < number_basisfunction; j++)
                {
                    shls[1] = j; dj = CINTcgto_spheric(j, basisfunctions);


                    if (this->Integral_Type == 100) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 3);
                        enviroment[1] = 0;
                        enviroment[2] = 0;
                        enviroment[3] = 0;
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_r_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_r_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }

                        int element = 0;
                        for (int component = 0; component < 3; component++) {
                            for (int j2 = 0; j2 < dj; j2++) {
                                for (int i2 = 0; i2 < di; i2++) {
                                    this->multiple_Integrals[component](block_start_i + i2, block_start_j + j2) = buf[element];
                                    element++;
                                }
                            }
                        }
                        free(buf);
                    }

                    if (this->Integral_Type == 101) {
                        buf = (double*)malloc(sizeof(double) * di * dj * 9);
                        enviroment[1] = 0;
                        enviroment[2] = 0;
                        enviroment[3] = 0;
                        if (this->Basisset.get_basisset_type() == 0) { cint1e_rr_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        else { cint1e_rr_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment); }
                        int element = 0;
                        for (int component = 0; component < 9; component++) {
                            for (int j2 = 0; j2 < dj; j2++) {
                                for (int i2 = 0; i2 < di; i2++) {
                                    this->multiple_Integrals[component](block_start_i + i2, block_start_j + j2) = buf[element];
                                    element++;
                                }
                            }
                        }
                        free(buf);
                    }
                    block_start_j += dj;
                }
                block_start_i += di;
            }

        }
        //std::cout << "Integral: " << this->Integrals(0, 0) << std::endl;

    }
}