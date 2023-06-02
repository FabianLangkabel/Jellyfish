#include <iostream>
#include "../header/integrator_gaussian_potential_gaussian.h"

namespace QC
{
    const double pi = 3.14159265359;

    inline int Integrator_Gaussian_Potential_Gaussian::fact(int n) {
        return n <= 1 ? 1 : n * fact(n - 1);
    }

    inline int Integrator_Gaussian_Potential_Gaussian::fact2(int n) {
        return n <= 1 ? 1 : n * fact2(n - 2);
    }

    int Integrator_Gaussian_Potential_Gaussian::binomial(int a, int b) {
        return fact(a) / (fact(b) * fact(a - b));
    }

    double Integrator_Gaussian_Potential_Gaussian::binomial_prefactor(int s, int ia, int ib, double xpa, double xpb) {
        int t;
        double sum = 0.;
        for (t = 0; t < s + 1; t++)
            if ((s - ia <= t) && (t <= ib))
                sum += binomial(ia, s - t) * binomial(ib, t) * pow(xpa, ia - s + t) * pow(xpb, ib - t);
        return sum;
    }



    double Integrator_Gaussian_Potential_Gaussian::norm(double exponent, int l, int m, int n)
    {
        double norm1 = pow((2 * exponent / pi), 0.75) * pow((4 * exponent), ((double)(l + m + n) / 2));
        double norm2 = sqrt(fact2(2 * l - 1) * fact2(2 * m - 1) * fact2(2 * n - 1));
        return norm1 / norm2;
    }

    double Integrator_Gaussian_Potential_Gaussian::overlap_1D(int a1, int a2, double exponent1, double exponent2, double potential_exponent, double center1, double center2, double center_product)
    {
        double overlap = 0;
        double gamma = exponent1 + exponent2 + potential_exponent;
        double x1 = center_product - center1;
        double x2 = center_product - center2;
        for (int i = 0; i < (1 + floor(0.5 * (a1 + a2))); i++) {
            overlap += binomial_prefactor(2 * i, a1, a2, x1, x2) * fact2(2 * i - 1) / pow(2 * gamma, i);
        }
        return overlap;
    }

    std::tuple<std::vector<double>, double> Integrator_Gaussian_Potential_Gaussian::gaussprod(std::vector<double> potential_center, double potential_exponent, std::vector<double> basisfunc1_center, double basisfunc1_exponent, std::vector<double> basisfunc2_center, double basisfunc2_exponent)
    {
        //Gauss Product Basisfunc1 * Basisfunc2
        std::vector<double> centerproduct_1;
        for (int i = 0; i < 3; i++) {
            centerproduct_1.push_back((basisfunc1_exponent * basisfunc1_center[i] + basisfunc2_exponent * basisfunc2_center[i]) / (basisfunc1_exponent + basisfunc2_exponent));
        }

        double prefactor1 = 0;
        for (int i = 0; i < 3; i++) {
            prefactor1 += (basisfunc1_center[i] - basisfunc2_center[i]) * (basisfunc1_center[i] - basisfunc2_center[i]);
        }
        prefactor1 = prefactor1 * (-basisfunc1_exponent * basisfunc2_exponent / (basisfunc1_exponent + basisfunc2_exponent));
        prefactor1 = exp(prefactor1);

        //Gauss Product from (Basisfunc1 * Basisfunc2) * QD Potential
        double exponent_temp = basisfunc1_exponent + basisfunc2_exponent;
        std::vector<double> centerproduct_final;
        for (int i = 0; i < 3; i++) {
            centerproduct_final.push_back((exponent_temp * centerproduct_1[i] + potential_exponent * potential_center[i]) / (exponent_temp + potential_exponent));
        }

        double prefactor2 = 0;
        for (int i = 0; i < 3; i++) {
            prefactor2 += (centerproduct_1[i] - potential_center[i]) * (centerproduct_1[i] - potential_center[i]);
        }
        prefactor2 = prefactor2 * (-exponent_temp * potential_exponent / (exponent_temp + potential_exponent));
        prefactor2 = exp(prefactor2);

        double prefactor_final = prefactor1 * prefactor2;

        return std::make_tuple(centerproduct_final, prefactor_final);
    }


    double Integrator_Gaussian_Potential_Gaussian::overlap(
        std::vector<double> potential_center, double potential_exponent,
        std::vector<double> basisfunc1_center, double basisfunc1_exponent, int basisfunc1_angular_moment_x, int basisfunc1_angular_moment_y, int basisfunc1_angular_moment_z,
        std::vector<double> basisfunc2_center, double basisfunc2_exponent, int basisfunc2_angular_moment_x, int basisfunc2_angular_moment_y, int basisfunc2_angular_moment_z)
    {
        std::vector<double> center_product;
        double coef;
        tie(center_product, coef) = gaussprod(potential_center, potential_exponent, basisfunc1_center, basisfunc1_exponent, basisfunc2_center, basisfunc2_exponent);

        double norm1 = norm(basisfunc1_exponent, basisfunc1_angular_moment_x, basisfunc1_angular_moment_y, basisfunc1_angular_moment_z);
        double norm2 = norm(basisfunc2_exponent, basisfunc2_angular_moment_x, basisfunc2_angular_moment_y, basisfunc2_angular_moment_z);
        double overlap_x = overlap_1D(basisfunc1_angular_moment_x, basisfunc2_angular_moment_x, basisfunc1_exponent, basisfunc2_exponent, potential_exponent, basisfunc1_center[0], basisfunc2_center[0], center_product[0]);
        double overlap_y = overlap_1D(basisfunc1_angular_moment_y, basisfunc2_angular_moment_y, basisfunc1_exponent, basisfunc2_exponent, potential_exponent, basisfunc1_center[1], basisfunc2_center[1], center_product[1]);
        double overlap_z = overlap_1D(basisfunc1_angular_moment_z, basisfunc2_angular_moment_z, basisfunc1_exponent, basisfunc2_exponent, potential_exponent, basisfunc1_center[2], basisfunc2_center[2], center_product[2]);
        double overlap = overlap_x * overlap_y * overlap_z * norm1 * norm2 * coef * pow((pi / (basisfunc1_exponent + basisfunc2_exponent + potential_exponent)), 1.5);
        return overlap;
    }

    double Integrator_Gaussian_Potential_Gaussian::potentialoverlap(std::tuple<Gaussian_Basisfunction, int> basisfunction1, std::tuple<Gaussian_Basisfunction, int> basisfunction2, std::vector<Potential_Gaussian> Gaussian_Potentials)
    {
        int ang_1, ang_2, func1_ang_x, func1_ang_y, func1_ang_z, func2_ang_x, func2_ang_y, func2_ang_z;
        Gaussian_Basisfunction basisfunc1, basisfunc2;
        std::tie (basisfunc1, ang_1) = basisfunction1;
        std::tie (basisfunc2, ang_2) = basisfunction2;
        func1_ang_x = basisfunc1.angular_moment_expanded_to_cartesian_angular_moments(ang_1)[0];
        func1_ang_y = basisfunc1.angular_moment_expanded_to_cartesian_angular_moments(ang_1)[1];
        func1_ang_z = basisfunc1.angular_moment_expanded_to_cartesian_angular_moments(ang_1)[2];
        func2_ang_x = basisfunc2.angular_moment_expanded_to_cartesian_angular_moments(ang_2)[0];
        func2_ang_y = basisfunc2.angular_moment_expanded_to_cartesian_angular_moments(ang_2)[1];
        func2_ang_z = basisfunc2.angular_moment_expanded_to_cartesian_angular_moments(ang_2)[2];

        double sum = 0;
        for (int potential = 0; potential < Gaussian_Potentials.size(); potential++) {
            for (int i = 0; i < basisfunc1.get_contraction(); i++) {
                for (int j = 0; j < basisfunc2.get_contraction(); j++) {
                    sum += basisfunc1.get_coefficient(i) * basisfunc2.get_coefficient(j) * Gaussian_Potentials[potential].get_coefficient() *
                        overlap(
                            Gaussian_Potentials[potential].get_center(), Gaussian_Potentials[potential].get_exponent(),
                            basisfunc1.get_center(), basisfunc1.get_exponent(i), func1_ang_x, func1_ang_y, func1_ang_z,
                            basisfunc2.get_center(), basisfunc2.get_exponent(i), func2_ang_x, func2_ang_y, func2_ang_z);
                }
            }
        }
        return sum;
    }

    void Integrator_Gaussian_Potential_Gaussian::compute()
    {
        int number_basisfunction = this->Basisset.get_Basisfunctionnumber_angular_expanded();

        this->Integrals = Eigen::MatrixXd::Zero(number_basisfunction, number_basisfunction);
        std::vector<std::tuple<Gaussian_Basisfunction, int>> all_Basisfunction_with_angular_expanded = this->Basisset.get_all_Basisfunction_with_angular_expanded();

        for (int i = 0; i < number_basisfunction; i++) {
            for (int j = 0; j < number_basisfunction; j++) {
                this->Integrals(i, j) = potentialoverlap(all_Basisfunction_with_angular_expanded[i], all_Basisfunction_with_angular_expanded[j], this->Gaussian_Potentials);
            }
        }
    }
}