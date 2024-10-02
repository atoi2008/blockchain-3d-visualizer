#ifndef ADAPTIVE_ECC_SOLVER_H
#define ADAPTIVE_ECC_SOLVER_H

#include <vector>
#include <random>
#include <Eigen/Dense>  // Assuming you use Eigen for linear algebra

class AdaptiveECCSolver {
public:
    AdaptiveECCSolver();
    void addTrainingData(const Eigen::VectorXd& features, double label);
    void trainModel();
    double predictNoiseThreshold(const Eigen::VectorXd& encodedData);

private:
    std::vector<Eigen::VectorXd> train_data;
    std::vector<double> train_labels;
    // Add members for the ML model (e.g., weights)
    bool initialized = false;
};

#endif // ADAPTIVE_ECC_SOLVER_H
