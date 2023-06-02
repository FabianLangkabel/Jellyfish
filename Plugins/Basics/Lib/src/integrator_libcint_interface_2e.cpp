#include <iostream>
#include "../header/integrator_libcint_interface_2e.h"

namespace QC
{
 
    void Integrator_libint_interface_2e::compute()
    {
        int number_atoms = this->Basisset.get_Basisfunctionnumber_angular_compact();
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


        for (int n = 0; n < number_basisfunction; n++)
        {
            basisfunctions[ATOM_OF + BAS_SLOTS * n] = n; //Corresponding Atom
            basisfunctions[ANG_OF + BAS_SLOTS * n] = this->Basisset.get_Basisfunction(n).get_total_angular_moment(); //Angular Moment
            basisfunctions[NPRIM_OF + BAS_SLOTS * n] = this->Basisset.get_Basisfunction(n).get_contraction(); //Number of primitiv GTO
            basisfunctions[NCTR_OF + BAS_SLOTS * n] = 1; //Number of contracted GTO
            basisfunctions[PTR_EXP + BAS_SLOTS * n] = offset; //Pointer für die 
            for (int m = 0; m < this->Basisset.get_Basisfunction(n).get_contraction(); m++)
            {
                enviroment[offset + m] = this->Basisset.get_Basisfunction(n).get_exponent(m);
            }
            offset += this->Basisset.get_Basisfunction(n).get_contraction();
            basisfunctions[PTR_COEFF + BAS_SLOTS * n] = offset;
            for (int m = 0; m < this->Basisset.get_Basisfunction(n).get_contraction(); m++)
            {
                enviroment[offset + m] = this->Basisset.get_Basisfunction(n).get_coefficient(m) * CINTgto_norm(basisfunctions[ANG_OF + BAS_SLOTS * n], enviroment[basisfunctions[PTR_EXP + BAS_SLOTS * n] + m]);
            }
            offset += this->Basisset.get_Basisfunction(n).get_contraction();
        }

        int i, j, k, l;
        int di, dj, dk, dl;
        int shls[4];
        double* buf;

      
        
        this->Integrals = Eigen::Tensor<double, 4>(number_basisfunction_angular_expanded, number_basisfunction_angular_expanded, number_basisfunction_angular_expanded, number_basisfunction_angular_expanded);


        int block_start_i = 0;
        for (i = 0; i < number_basisfunction; i++)
        {
            shls[0] = i; di = CINTcgto_spheric(i, basisfunctions);
            int block_start_j = 0;
            for (j = 0; j < number_basisfunction; j++)
            {
                shls[1] = j; dj = CINTcgto_spheric(j, basisfunctions);
                int block_start_k = 0;
                for (k = 0; k < number_basisfunction; k++)
                {
                    shls[2] = k; dk = CINTcgto_spheric(k, basisfunctions);
                    int block_start_l = 0;
                    for (l = 0; l < number_basisfunction; l++)
                    {
                        shls[3] = l; dl = CINTcgto_spheric(l, basisfunctions);

                        buf = (double*)malloc(sizeof(double) * di * dj * dk * dl);

                        if (this->Integral_Type == 0) {
                            if (this->Basisset.get_basisset_type() == 0) { cint2e_sph(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment, NULL); }
                            else { cint2e_cart(buf, shls, atoms, number_atoms, basisfunctions, number_basisfunction, enviroment, NULL); }
                        }
                        
                        int element = 0;
                        for (int l2 = 0; l2 < dl; l2++) {
                            for (int k2 = 0; k2 < dk; k2++) {
                                for (int j2 = 0; j2 < dj; j2++) {
                                    for (int i2 = 0; i2 < di; i2++) {
                                        this->Integrals(block_start_i + i2, block_start_j + j2, block_start_k + k2, block_start_l + l2) = buf[element];
                                        element++;
                                    }
                                }
                            }
                        }

                        free(buf);
                        block_start_l += dl;
                    }
                    block_start_k += dk;
                }
                block_start_j += dj;
            }
            block_start_i += di;
        }
        
    }
}