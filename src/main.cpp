#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Function to generate random coefficients
std::vector<int> generateRandomCoefficients(int t, int MOD) {
    std::vector<int> coefficients(t);
    for (int i = 0; i < t; ++i) {
        coefficients[i] = rand() % MOD;
    }
    return coefficients;
}

// Function to evaluate polynomial at x
int evaluatePolynomial(const std::vector<int>& coefficients, int x, int MOD, int secret) {
    int result = 0;
    int power = 1;
    for (int coeff : coefficients) {
        result = (result + coeff * power) % MOD;
        power = (power * x) % MOD;
    }
    result += secret;

    return result;
}

// Function to generate shares
std::vector<std::pair<int, int>> generateShares(int secret, int n, int t, int MOD) {
    // The first coefficient is the secret
    std::vector<int> coefficients = generateRandomCoefficients(t, MOD);

    std::vector<std::pair<int, int>> shares;
    for (int i = 1; i <= n; ++i) {
        int x = i;
        int y = evaluatePolynomial(coefficients, x, MOD, secret);
        shares.push_back({x, y});
    }
    return shares;
}

// Function to perform modular inverse
int modInverse(int a, int MOD) {
    int m0 = MOD, t, q;
    int x0 = 0, x1 = 1;

    if (MOD == 1) return 0;

    while (a > 1) {
        q = a / MOD;
        t = MOD;

        MOD = a % MOD, a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;

    return x1;
}

// Function to reconstruct the secret using Lagrange interpolation
int reconstructSecret(const std::vector<std::pair<int, int>>& shares, int MOD) {
    int secret = 0;
    int k = shares.size();

    for (int i = 0; i < k; ++i) {
        int xi = shares[i].first;
        int yi = shares[i].second;

        int term = yi;
        for (int j = 0; j < k; ++j) {
            if (i != j) {
                int xj = shares[j].first;
                term = (term * (MOD - xj)) % MOD;
                term = (term * modInverse((xi - xj + MOD) % MOD, MOD)) % MOD;
            }
        }
        secret = (secret + term) % MOD;
    }

    return secret;
}

bool isPrime(int number) {
    if (number <= 1) {
        return false;
    }
    if (number <= 3) {
        return true;
    }
    if (number % 2 == 0 || number % 3 == 0) {
        return false;
    }
    for (int i = 5; i * i <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

int main() {
    int n, MOD, secret, t;

    std::cout << "Enter the number of people: ";
    std::cin >> n;
    std::cout << "Enter the modulus (MOD): ";
    std::cin >> MOD;

    if (!isPrime(MOD)) {
        std::cerr << "MOD must be a prime number." << std::endl;
    }

    std::cout << "Enter the secret: ";
    std::cin >> secret;
    std::cout << "Enter the threshold (t): ";
    std::cin >> t;

    if (t > n) {
        std::cerr << "Threshold t cannot be greater than the number of people n." << std::endl;
        return 1;
    }

    srand(time(0));

    std::vector<std::pair<int, int>> shares = generateShares(secret, n, t, MOD);

    std::cout << "Generated shares: " << std::endl;
    for (const auto& share : shares) {
        std::cout << "x: " << share.first << ", y: " << share.second << std::endl;
    }

    std::cout << "\nEnter at least " << t << " shares to reconstruct the secret:" << std::endl;
    std::vector<std::pair<int, int>> inputShares;
    for (int i = 0; i < t; ++i) {
        int x, y;
        std::cout << "Enter share " << i + 1 << " (x y): ";
        std::cin >> x >> y;
        inputShares.push_back({x, y});
    }

    int reconstructedSecret = reconstructSecret(inputShares, MOD);
    std::cout << "Reconstructed secret: " << reconstructedSecret << std::endl;

    return 0;
}