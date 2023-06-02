#include "../header/jordan_wigner.h"
namespace QC
{
	void MO_Integrals_to_JW_Pauli_Operator::compute(bool translate_one_electron_integrals, bool translate_two_electron_integrals)
	{
        this->fermionic_operator.clear();

        if (translate_one_electron_integrals)
        {
            for (int i = 0; i < this->one_electron_integrals.rows(); i++) {
                for (int j = 0; j < this->one_electron_integrals.cols(); j++) {
                    if (this->one_electron_integrals(i, j) != 0)
                    {
                        one_e_integral_to_one_e_spin_Orbitals(i, j, this->one_electron_integrals(i, j));
                    }
                }
            }
        }
        
        if (translate_two_electron_integrals)
        {
            for (int i = 0; i < this->two_electron_integrals.dimension(0); i++) {
                for (int j = 0; j < this->two_electron_integrals.dimension(1); j++) {
                    for (int k = 0; k < this->two_electron_integrals.dimension(2); k++) {
                        for (int l = 0; l < this->two_electron_integrals.dimension(3); l++) {
                            if (this->two_electron_integrals(i, j, k, l) != 0)
                            {
                                two_e_integral_to_two_e_spin_Orbitals(k, j, i, l, this->two_electron_integrals(i, j, k, l));
                            }
                        }
                    }
                }
            }
        }
        
        sort_fermionic_terms_by_sym();
        //print_fermionic_hamiltonian();
        jordan_wigner_transformation_for_operator();
        //print_JW_hamiltonian();

        JW_operator_to_pauli_operator();
        this->Pauli_Operator.set_number_qubits(this->one_electron_integrals.rows() * 2);
        //this->Pauli_Hamiltonian.print_Pauli_Hamiltonian();
	}

	void MO_Integrals_to_JW_Pauli_Operator::one_e_integral_to_one_e_spin_Orbitals(int i, int j, double coef)
	{
        std::vector<std::vector<int>> spin_confs = enumerate_one_e_integral_symmetries_and_spin(i, j);

        for (int i = 0; i < spin_confs.size(); i++) {
            int p = spin_confs[i][0];
            int q = spin_confs[i][1];
            std::vector<int> orbs = { p, q };
            if (p == q)
            {
                add_term_to_fermionic_operator(orbs, coef);
            }
            else if (p < q)
            {
                add_term_to_fermionic_operator(orbs, coef * 2);
            }
        }
	}

    void MO_Integrals_to_JW_Pauli_Operator::two_e_integral_to_two_e_spin_Orbitals(int i, int j, int k, int l, double coef)
    {
        std::vector<std::vector<int>> spin_confs = enumerate_two_e_integral_symmetries_and_spin(i, j, k, l);

        for (int i = 0; i < spin_confs.size(); i++) {
            int p = spin_confs[i][0];
            int q = spin_confs[i][1];
            int r = spin_confs[i][2];
            int s = spin_confs[i][3];

            if (p == s && q == r && p < q)
            {
                std::vector<int> orbs = { p, q, r, s };
                add_term_to_fermionic_operator(orbs, coef);
            }
            else if (p == r && q == s && p < q)
            {
                std::vector<int> orbs = { p, q, s, r };
                add_term_to_fermionic_operator(orbs, -coef);
            }
            else if (q == r && p < s && r != s && p != q)
            {
                if (r < s) 
                {
                    if (p < q)
                    {
                        std::vector<int> orbs = { p, q, s, r };
                        add_term_to_fermionic_operator(orbs, -2 * coef);
                    }
                    else
                    {
                        std::vector<int> orbs = { q, p, s, r };
                        add_term_to_fermionic_operator(orbs, 2 * coef);
                    }
                }
                else
                {
                    if (p < q)
                    {
                        std::vector<int> orbs = { p, q, r, s };
                        add_term_to_fermionic_operator(orbs, 2 * coef);
                    }
                    else
                    {
                        std::vector<int> orbs = { q, p, r, s };
                        add_term_to_fermionic_operator(orbs, -2 * coef);
                    }
                }
            }
            else if (q == s && p < r && r != s && p != s)
            {
                if (p < q)
                {
                    if (r > q)
                    {
                        std::vector<int> orbs = { p, q, r, s };
                        add_term_to_fermionic_operator(orbs, 2 * coef);
                    }
                    else
                    {
                        std::vector<int> orbs = { p, q, s, r };
                        add_term_to_fermionic_operator(orbs, -2 * coef);
                    }
                }
                else
                {
                    std::vector<int> orbs = { q, p, r, s };
                    add_term_to_fermionic_operator(orbs, -2 * coef);
                }
            }
            else if (p < q && p < r && p < s && q != r && q != s && r != s)
            {
                if (r < s)
                {
                    std::vector<int> orbs = { p, q, s, r };
                    add_term_to_fermionic_operator(orbs, -2 * coef);
                }
                else
                {
                    std::vector<int> orbs = { p, q, r, s };
                    add_term_to_fermionic_operator(orbs, 2 * coef);
                }
            }
        }
    }

    std::vector<std::vector<int>> MO_Integrals_to_JW_Pauli_Operator::enumerate_one_e_integral_symmetries_and_spin(int i, int j) {

        std::vector<std::vector<int>> result;

		std::vector<std::vector<int>> symmetries;
        symmetries.push_back({ i,j });
		//symmetries.push_back({ j,i });
        std::sort(symmetries.begin(), symmetries.end());
        symmetries.erase(std::unique(symmetries.begin(), symmetries.end()), symmetries.end());

        for (int iterator = 0; iterator < symmetries.size(); iterator++) {
            int nSpins = 2;
            int number_indices = 2;
            int nSpinOrbitalArrays = pow(2, number_indices / 2);
            std::vector<std::vector<int>> spinOrbitalArrayOfArray(number_indices, std::vector<int>(number_indices));
            for (int idx = 0; idx < nSpinOrbitalArrays; idx++)
            {
                for (int idxOrbital = 0; idxOrbital < number_indices / 2; idxOrbital++)
                {
                    int fst = idxOrbital;
                    int lst = number_indices - idxOrbital - 1;
                    int spin = (idx / (int)pow(nSpins, idxOrbital)) % nSpins;
                    if (spin == 0) {
                        spinOrbitalArrayOfArray[idx][fst] = symmetries[iterator][fst] * 2;
                        spinOrbitalArrayOfArray[idx][lst] = symmetries[iterator][lst] * 2;
                    }
                    else {
                        spinOrbitalArrayOfArray[idx][fst] = symmetries[iterator][fst] * 2 + 1;
                        spinOrbitalArrayOfArray[idx][lst] = symmetries[iterator][lst] * 2 + 1;
                    }
                }
            }
            for (int spin_conf = 0; spin_conf < spinOrbitalArrayOfArray.size(); spin_conf++)
            {
                result.push_back(spinOrbitalArrayOfArray[spin_conf]);
            }
        }
        return result;
	}

    std::vector<std::vector<int>> MO_Integrals_to_JW_Pauli_Operator::enumerate_two_e_integral_symmetries_and_spin(int i, int j, int k, int l) {

        std::vector<std::vector<int>> result;

        std::vector<std::vector<int>> symmetries;
        symmetries.push_back({ i, j, k, l });
        //symmetries.push_back({ j, i, l, k });
        //symmetries.push_back({ k, l, i, j });
        //symmetries.push_back({ l, k, j, i });
        //symmetries.push_back({ i, k, j, l });
        //symmetries.push_back({ k, i, l, j });
        //symmetries.push_back({ j, l, i, k });
        //symmetries.push_back({ l, j, k, i });
        std::sort(symmetries.begin(), symmetries.end());
        symmetries.erase(std::unique(symmetries.begin(), symmetries.end()), symmetries.end());

        for (int iterator = 0; iterator < symmetries.size(); iterator++) {
            int nSpins = 2;
            int number_indices = 4;
            int nSpinOrbitalArrays = pow(2, number_indices / 2);
            std::vector<std::vector<int>> spinOrbitalArrayOfArray(number_indices, std::vector<int>(number_indices));
            for (int idx = 0; idx < nSpinOrbitalArrays; idx++)
            {
                for (int idxOrbital = 0; idxOrbital < number_indices / 2; idxOrbital++)
                {
                    int fst = idxOrbital;
                    int lst = number_indices - idxOrbital - 1;
                    int spin = (idx / (int)pow(nSpins, idxOrbital)) % nSpins;
                    if (spin == 0) {
                        spinOrbitalArrayOfArray[idx][fst] = symmetries[iterator][fst] * 2;
                        spinOrbitalArrayOfArray[idx][lst] = symmetries[iterator][lst] * 2;
                    }
                    else {
                        spinOrbitalArrayOfArray[idx][fst] = symmetries[iterator][fst] * 2 + 1;
                        spinOrbitalArrayOfArray[idx][lst] = symmetries[iterator][lst] * 2 + 1;
                    }
                }
            }
            for (int spin_conf = 0; spin_conf < spinOrbitalArrayOfArray.size(); spin_conf++)
            {
                result.push_back(spinOrbitalArrayOfArray[spin_conf]);
            }
        }
        return result;
    }

    void MO_Integrals_to_JW_Pauli_Operator::add_term_to_fermionic_operator(std::vector<int> indecies, double coef) {

        for (int i = 0; i < this->fermionic_operator.size(); i++)
        {
            std::vector<int> indecies_ref;
            double coef_ref;
            std::tie(indecies_ref, coef_ref) = this->fermionic_operator[i];
            if (indecies_ref.size() == 2 && indecies.size() == 2 && indecies_ref[0] == indecies[0] && indecies_ref[1] == indecies[1])
            {
                double new_coef = coef + coef_ref;
                this->fermionic_operator[i] = std::make_tuple(indecies, new_coef);
                return;
            }
            if (indecies_ref.size() == 4 && indecies.size() == 4 && indecies_ref[0] == indecies[0] && indecies_ref[1] == indecies[1] && indecies_ref[2] == indecies[2] && indecies_ref[3] == indecies[3])
            {
                double new_coef = coef + coef_ref;
                this->fermionic_operator[i] = std::make_tuple(indecies, new_coef);
                return;
            }
        }
        this->fermionic_operator.push_back(std::make_tuple(indecies, coef));
        return;
    }

    void MO_Integrals_to_JW_Pauli_Operator::sort_fermionic_terms_by_sym()
    {
        this->fermionic_terms_pp_sym.clear();
        this->fermionic_terms_pq_sym.clear();
        this->fermionic_terms_pqqp_sym.clear();
        this->fermionic_terms_pqqr_sym.clear();
        this->fermionic_terms_pqrs_sym.clear();

        for (int i = 0; i < this->fermionic_operator.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[i];
            if (indecies.size() == 2)
            {
                if (indecies[0] == indecies[1]) { fermionic_terms_pp_sym.push_back(i); }
                else { this->fermionic_terms_pq_sym.push_back(i); }
            }
            else if (indecies.size() == 4)
            {
                int p = indecies[0];
                int q = indecies[1];
                int r = indecies[2];
                int s = indecies[3];
                if (p == s && q == r || p == r && q == s)
                {
                    this->fermionic_terms_pqqp_sym.push_back(i);
                }
                else if (p == r || p == s || q == r || q == s)
                {
                    this->fermionic_terms_pqqr_sym.push_back(i);
                }
                else
                {
                    this->fermionic_terms_pqrs_sym.push_back(i);
                }
            }
        }
    }

    void MO_Integrals_to_JW_Pauli_Operator::print_fermionic_operator()
    {
        std::cout << "PP-Symmetrie (" << this->fermionic_terms_pp_sym.size() << " terms)" << std::endl;
        for (int i = 0; i < this->fermionic_terms_pp_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pp_sym[i]];
            std::cout << "    " << indecies[0] << "a " << indecies[1] << "b, " << coef << std::endl;
        }

        std::cout << "PQ-Symmetrie (" << this->fermionic_terms_pq_sym.size() << " terms)" << std::endl;
        for (int i = 0; i < this->fermionic_terms_pq_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pq_sym[i]];
            std::cout << "    " << indecies[0] << "a " << indecies[1] << "b, " << coef << std::endl;
        }

        std::cout << "PQQP-Symmetrie (" << this->fermionic_terms_pqqp_sym.size() << " terms)" << std::endl;
        for (int i = 0; i < this->fermionic_terms_pqqp_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pqqp_sym[i]];
            std::cout << "    " << indecies[0] << "a " << indecies[1] << "a " << indecies[2] << "b " << indecies[3] << "b, " << coef << std::endl;
        }

        std::cout << "PQQR-Symmetrie (" << this->fermionic_terms_pqqr_sym.size() << " terms)" << std::endl;
        for (int i = 0; i < this->fermionic_terms_pqqr_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pqqr_sym[i]];
            std::cout << "    " << indecies[0] << "a " << indecies[1] << "a " << indecies[2] << "b " << indecies[3] << "b, " << coef << std::endl;
        }

        std::cout << "PQRS-Symmetrie (" << this->fermionic_terms_pqrs_sym.size() << " terms)" << std::endl;
        for (int i = 0; i < this->fermionic_terms_pqrs_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pqrs_sym[i]];
            std::cout << "    " << indecies[0] << "a " << indecies[1] << "a " << indecies[2] << "b " << indecies[3] << "b, " << coef << std::endl;
        }
    }

    void MO_Integrals_to_JW_Pauli_Operator::jordan_wigner_transformation_for_operator()
    {
        JW_Identity = e0;
        JW_z_term.clear();
        JW_pq_term.clear();
        JW_zz_term.clear();
        JW_pqqr_term.clear();
        JW_v01234_term.clear();

        for (int i = 0; i < this->fermionic_terms_pp_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pp_sym[i]];
            add_single_pauli_term(0, {}, { 0.5 * coef });
            add_single_pauli_term(1, { indecies[0] }, { -0.5 * coef });
        }

        for (int i = 0; i < this->fermionic_terms_pq_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pq_sym[i]];
            add_single_pauli_term(2, { indecies[0], indecies[1] }, { 0.25 * coef });
        }

        for (int i = 0; i < this->fermionic_terms_pqqp_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pqqp_sym[i]];
            add_single_pauli_term(0, {}, { 0.25 * coef });
            add_single_pauli_term(3, { indecies[0], indecies[1] }, { 0.25 * coef });
            add_single_pauli_term(1, { indecies[0] }, { -0.25 * coef });
            add_single_pauli_term(1, { indecies[1] }, { -0.25 * coef });
        }

        for (int i = 0; i < this->fermionic_terms_pqqr_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pqqr_sym[i]];

            int multiplier = 1;
            //std::vector<int> indecies_new = indecies;
            if (indecies[0] == indecies[3])
            {
                //QPRQ to PQQR
                indecies[0] = indecies[1];
                indecies[1] = indecies[3];
                indecies[3] = indecies[2];
                indecies[2] = indecies[1];
            }
            else if (indecies[1] == indecies[3])
            {
                //PQRQ to PQQR
                indecies[3] = indecies[2];
                indecies[2] = indecies[1];
                multiplier = -1;
            }

            add_single_pauli_term(4, indecies, { -0.125 * multiplier * coef });
            add_single_pauli_term(2, { indecies[0], indecies[3] }, { 0.125 * multiplier * coef });
        }

        for (int i = 0; i < this->fermionic_terms_pqrs_sym.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->fermionic_operator[fermionic_terms_pqrs_sym[i]];

            coef = coef * 0.0625;
            std::vector<int>indecies_sorted = indecies;
            std::sort(indecies_sorted.begin(), indecies_sorted.end());
            std::vector<double> h123 = { 0, 0, 0 };
            std::vector<double> v0123 = { 0, 0, 0, 0 };

            if (indecies[0] == indecies_sorted[0] && indecies[1] == indecies_sorted[2] && indecies[2] == indecies_sorted[3] && indecies[3] == indecies_sorted[1])
            {
                h123 = {0, 0, coef };
            }
            else if (indecies[0] == indecies_sorted[0] && indecies[1] == indecies_sorted[1] && indecies[2] == indecies_sorted[3] && indecies[3] == indecies_sorted[2])
            {
                h123 = { -coef, 0, 0 };
            }
            else if (indecies[0] == indecies_sorted[0] && indecies[1] == indecies_sorted[3] && indecies[2] == indecies_sorted[2] && indecies[3] == indecies_sorted[1])
            {
                h123 = { 0, -coef, 0 };
            }
            else
            {
                h123 = { 0, 0, 0 };
            }

            v0123 = {
                -h123[0] - h123[1] + h123[2],
                h123[0] - h123[1] + h123[2],
                -h123[0] - h123[1] - h123[2],
                -h123[0] + h123[1] + h123[2],
            };

            if (v0123[0] != 0 || v0123[1] != 0 || v0123[2] != 0 || v0123[3] != 0)
            {
                add_single_pauli_term(5, indecies_sorted, v0123);
            }
        }
    }

    void MO_Integrals_to_JW_Pauli_Operator::add_single_pauli_term(int termtype, std::vector<int> indicies, std::vector<double> coef)
    {
        //termtypes
        //0 = Identity
        //1 = z
        //2 = pq
        //3 = zz
        //4 = pqqr
        //5 = v01234

        if (termtype == 0)
        {
            this->JW_Identity += coef[0];
            return;
        }
        else if (termtype == 1)
        {
            for (int i = 0; i < this->JW_z_term.size(); i++)
            {
                int indicies_ref;
                double coef_ref;
                std::tie(indicies_ref, coef_ref) = JW_z_term[i];
                if (indicies_ref == indicies[0])
                {
                    JW_z_term[i] = std::make_tuple(indicies_ref, coef_ref + coef[0]);
                    return;
                }
            }
            this->JW_z_term.push_back(std::make_tuple(indicies[0], coef[0]));
        }
        else if (termtype == 2)
        {
            for (int i = 0; i < this->JW_pq_term.size(); i++)
            {
                std::vector<int> indicies_ref;
                double coef_ref;
                std::tie(indicies_ref, coef_ref) = JW_pq_term[i];
                if (indicies_ref[0] == indicies[0] && indicies_ref[1] == indicies[1])
                {
                    JW_pq_term[i] = std::make_tuple(indicies_ref, coef_ref + coef[0]);
                    return;
                }
            }
            this->JW_pq_term.push_back(std::make_tuple(indicies, coef[0]));
        }
        else if (termtype == 3)
        {
            for (int i = 0; i < this->JW_zz_term.size(); i++)
            {
                std::vector<int> indicies_ref;
                double coef_ref;
                std::tie(indicies_ref, coef_ref) = JW_zz_term[i];
                if (indicies_ref[0] == indicies[0] && indicies_ref[1] == indicies[1])
                {
                    JW_zz_term[i] = std::make_tuple(indicies_ref, coef_ref + coef[0]);
                    return;
                }
            }
            this->JW_zz_term.push_back(std::make_tuple(indicies, coef[0]));
        }
        else if (termtype == 4)
        {
            for (int i = 0; i < this->JW_pqqr_term.size(); i++)
            {
                std::vector<int> indicies_ref;
                double coef_ref;
                std::tie(indicies_ref, coef_ref) = JW_pqqr_term[i];
                if (indicies_ref[0] == indicies[0] && indicies_ref[1] == indicies[1] && indicies_ref[2] == indicies[2] && indicies_ref[3] == indicies[3])
                {
                    JW_pqqr_term[i] = std::make_tuple(indicies_ref, coef_ref + coef[0]);
                    return;
                }
            }
            this->JW_pqqr_term.push_back(std::make_tuple(indicies, coef[0]));
        }
        else if (termtype == 5)
        {
            for (int i = 0; i < this->JW_v01234_term.size(); i++)
            {
                std::vector<int> indicies_ref;
                std::vector<double> coef_ref;
                std::tie(indicies_ref, coef_ref) = JW_v01234_term[i];
                if (indicies_ref[0] == indicies[0] && indicies_ref[1] == indicies[1] && indicies_ref[2] == indicies[2] && indicies_ref[3] == indicies[3])
                {
                    std::vector<double> coef_new;
                    coef_new.push_back(coef_ref[0] + coef[0]);
                    coef_new.push_back(coef_ref[1] + coef[1]);
                    coef_new.push_back(coef_ref[2] + coef[2]);
                    coef_new.push_back(coef_ref[3] + coef[3]);
                    JW_v01234_term[i] = std::make_tuple(indicies_ref, coef_new);
                    return;
                }
            }
            this->JW_v01234_term.push_back(std::make_tuple(indicies, coef));
        }
    }

    void MO_Integrals_to_JW_Pauli_Operator::print_JW_operator()
    {
        std::cout << "Identity (1 term)" << std::endl;
        std::cout << "    " << "Identity: " << JW_Identity << std::endl;

        std::cout << "Z (" << this->JW_z_term.size() << " terms)" << std::endl;
        for (int i = 0; i < this->JW_z_term.size(); i++)
        {
            int indecie;
            double coef;
            std::tie(indecie, coef) = this->JW_z_term[i];
            std::cout << "    " << indecie << " : " << coef << std::endl;
        }

        std::cout << "PQ (" << this->JW_pq_term.size() << " terms)" << std::endl;
        for (int i = 0; i < this->JW_pq_term.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->JW_pq_term[i];
            std::cout << "    " << indecies[0] << " " << indecies[1] << " : " << coef << std::endl;
        }

        std::cout << "ZZ (" << this->JW_zz_term.size() << " terms)" << std::endl;
        for (int i = 0; i < this->JW_zz_term.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->JW_zz_term[i];
            std::cout << "    " << indecies[0] << " " << indecies[1] << " : " << coef << std::endl;
        }

        std::cout << "PQQR (" << this->JW_pqqr_term.size() << " terms)" << std::endl;
        for (int i = 0; i < this->JW_pqqr_term.size(); i++)
        {
            std::vector<int> indecies;
            double coef;
            std::tie(indecies, coef) = this->JW_pqqr_term[i];
            std::cout << "    " << indecies[0] << " " << indecies[1] << " " << indecies[2] << " " << indecies[3] << " : " << coef << std::endl;
        }

        std::cout << "v01234 (" << this->JW_v01234_term.size() << " terms)" << std::endl;
        for (int i = 0; i < this->JW_v01234_term.size(); i++)
        {
            std::vector<int> indecies;
            std::vector<double> coef;
            std::tie(indecies, coef) = this->JW_v01234_term[i];
            std::cout << "    " << indecies[0] << " " << indecies[1] << " " << indecies[2] << " " << indecies[3] << " : " << coef[0] << " " << coef[1] << " " << coef[2] << " " << coef[3] << std::endl;
        }
    }
    
    void MO_Integrals_to_JW_Pauli_Operator::JW_operator_to_pauli_operator()
    {
        this->Pauli_Operator.clear();
        this->Pauli_Operator.set_Identity(this->JW_Identity);
        // Z-Terms
        for (int i = 0; i < this->JW_z_term.size(); i++)
        {
            int qubit;
            double coef;
            std::tie(qubit, coef) = JW_z_term[i];
            QC::Pauli_String new_Pauli_String;
            new_Pauli_String.set_coefficient(coef);
            new_Pauli_String.set_PauliOperations({ "Z" });
            new_Pauli_String.set_corresponding_Qubits({ qubit });
            this->Pauli_Operator.add_pauli_string(new_Pauli_String);
        }

        // ZZ-Terms
        for (int i = 0; i < this->JW_zz_term.size(); i++)
        {
            std::vector<int> qubits;
            double coef;
            std::tie(qubits, coef) = JW_zz_term[i];
            QC::Pauli_String new_Pauli_String;
            new_Pauli_String.set_coefficient(coef);
            new_Pauli_String.set_PauliOperations({ "Z", "Z" });
            new_Pauli_String.set_corresponding_Qubits({ qubits[0], qubits[1] });
            this->Pauli_Operator.add_pauli_string(new_Pauli_String);
        }

        // PQ-Terms
        for (int i = 0; i < this->JW_pq_term.size(); i++)
        {
            std::vector<int> qubits;
            double coef;
            std::tie(qubits, coef) = JW_pq_term[i];

            std::vector<std::string> op1_pauli = { "X", "X" };
            std::vector<std::string> op2_pauli = { "Y", "Y" };
            std::vector<int> qubit_seq = { qubits[0], qubits[1] };
            for (int i = qubits[0] + 1; i < qubits[1]; i++)
            {
                op1_pauli.push_back("Z");
                op2_pauli.push_back("Z");
                qubit_seq.push_back(i);
            }
            {
                QC::Pauli_String new_Pauli_String;
                new_Pauli_String.set_coefficient(coef);
                new_Pauli_String.set_PauliOperations(op1_pauli);
                new_Pauli_String.set_corresponding_Qubits(qubit_seq);
                this->Pauli_Operator.add_pauli_string(new_Pauli_String);
            }
            {
                QC::Pauli_String new_Pauli_String;
                new_Pauli_String.set_coefficient(coef);
                new_Pauli_String.set_PauliOperations(op2_pauli);
                new_Pauli_String.set_corresponding_Qubits(qubit_seq);
                this->Pauli_Operator.add_pauli_string(new_Pauli_String);
            }
        }

        // PQQR-Terms
        for (int i = 0; i < this->JW_pqqr_term.size(); i++)
        {
            std::vector<int> qubits;
            double coef;
            std::tie(qubits, coef) = JW_pqqr_term[i];

            std::vector<std::string> op1_pauli = { "X", "X" };
            std::vector<std::string> op2_pauli = { "Y", "Y" };
            std::vector<int> qubit_seq = { qubits[0], qubits[3] };

            if (qubits[0] < qubits[1] && qubits[1] < qubits[3])
            {
                for (int i = qubits[0] + 1; i < qubits[3]; i++)
                {
                    if (i != qubits[1])
                    {
                        op1_pauli.push_back("Z");
                        op2_pauli.push_back("Z");
                        qubit_seq.push_back(i);
                    }
                }
            }
            else
            {
                for (int i = qubits[0] + 1; i < qubits[3]; i++)
                {
                    op1_pauli.push_back("Z");
                    op2_pauli.push_back("Z");
                    qubit_seq.push_back(i);
                }
                op1_pauli.push_back("Z");
                op2_pauli.push_back("Z");
                qubit_seq.push_back(qubits[1]);
            }
            {
                QC::Pauli_String new_Pauli_String;
                new_Pauli_String.set_coefficient(coef);
                new_Pauli_String.set_PauliOperations(op1_pauli);
                new_Pauli_String.set_corresponding_Qubits(qubit_seq);
                this->Pauli_Operator.add_pauli_string(new_Pauli_String);
            }
            {
                QC::Pauli_String new_Pauli_String;
                new_Pauli_String.set_coefficient(coef);
                new_Pauli_String.set_PauliOperations(op2_pauli);
                new_Pauli_String.set_corresponding_Qubits(qubit_seq);
                this->Pauli_Operator.add_pauli_string(new_Pauli_String);
            }
        }

        // v01234-Terms
        for (int i = 0; i < this->JW_v01234_term.size(); i++)
        {
            std::vector<int> qubits;
            std::vector<double> coef;
            std::tie(qubits, coef) = JW_v01234_term[i];
            std::vector<std::vector<std::string>> ops;
            ops.push_back({ "X", "X", "X", "X" });
            ops.push_back({ "X", "X", "Y", "Y" });
            ops.push_back({ "X", "Y", "X", "Y" });
            ops.push_back({ "Y", "X", "X", "Y" });
            ops.push_back({ "Y", "Y", "Y", "Y" });
            ops.push_back({ "Y", "Y", "X", "X" });
            ops.push_back({ "Y", "X", "Y", "X" });
            ops.push_back({ "X", "Y", "Y", "X" });

            for (int i = 0; i < ops.size(); i++)
            {
                std::vector<std::string> op = ops[i];
                std::vector<int> qubit_seq = { qubits[0], qubits[1], qubits[2], qubits[3] };
                double coef_in_loop = coef[i % 4];
                for (int j = qubits[0] + 1; j < qubits[1]; j++)
                {
                    op.push_back("Z");
                    qubit_seq.push_back(j);
                }
                for (int j = qubits[2] + 1; j < qubits[3]; j++)
                {
                    op.push_back("Z");
                    qubit_seq.push_back(j);
                }
                if (coef_in_loop != 0.0)
                {
                    QC::Pauli_String new_Pauli_String;
                    new_Pauli_String.set_coefficient(coef_in_loop);
                    new_Pauli_String.set_PauliOperations(op);
                    new_Pauli_String.set_corresponding_Qubits(qubit_seq);
                    this->Pauli_Operator.add_pauli_string(new_Pauli_String);
                }
            }
        }

    }

    std::vector<QC::Pauli_Operator> JW_Laser_Pauli_Operator::get_Pauli_Operators_at_time(double time)
    {
        if (!this->transition_matrices_uptodate) { this->calculate_transition_matrices(); }
        std::vector<double> field_strenghts = this->Laser->get_field_strengths(time);
        std::vector<QC::Pauli_Operator> Pauli_Operators_at_time;

        QC::MO_Integrals_to_JW_Pauli_Operator Transformation;
        Transformation.set_one_electron_integrals(this->transition_matrix_x * field_strenghts[0]);
        Transformation.set_e0(0);
        Pauli_Operators_at_time.push_back(Transformation.get_Pauli_Operator());

        Transformation.set_one_electron_integrals(this->transition_matrix_y * field_strenghts[1]);
        Transformation.set_e0(0);
        Transformation.compute(true, false);
        Pauli_Operators_at_time.push_back(Transformation.get_Pauli_Operator());

        Transformation.set_one_electron_integrals(this->transition_matrix_z * field_strenghts[2]);
        Transformation.set_e0(0);
        Transformation.compute(true, false);
        Pauli_Operators_at_time.push_back(Transformation.get_Pauli_Operator());

        /*
        Transformation.set_one_electron_integrals(this->transition_matrix_z);
        Transformation.set_e0(0);
        Transformation.compute(true, false);
        Transformation.get_Pauli_Operator().print_Pauli_Operator();
        */

        return Pauli_Operators_at_time;
    }
}