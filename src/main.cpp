#include <bits/stdc++.h>
using namespace std;

// ============================
// Cargo structure
// ============================
struct Cargo {
    double volume;
    double weight;
    double profit;
};

// ============================
// Compartment structure
// ============================
struct Compartment {
    double maxVolume;
    double maxWeight;
};

// ============================
// Problem Data
// ============================
const int NUM_CARGOS = 4;
const int NUM_COMPARTMENTS = 3;

Cargo cargos[NUM_CARGOS] = {
    {480, 18, 310},
    {650, 15, 380},
    {580, 23, 350},
    {390, 12, 285}
};

Compartment compartments[NUM_COMPARTMENTS] = {
    {6800, 10},   // Front
    {8700, 16},   // Center
    {5300, 8}     // Rear
};

// Genetic Algorithm parameters
const int POP_SIZE = 100;
const int GENERATIONS = 500;
const double MUTATION_RATE = 0.1;
const double CROSSOVER_RATE = 0.8;

// ============================
// Chromosome representation:
// A chromosome is a vector with NUM_CARGOS * NUM_COMPARTMENTS elements
// Each element = tons of cargo allocated to a given compartment
// ============================
struct Chromosome {
    vector<double> genes;
    double fitness;
};

// ============================
// Fitness function
// ============================
double evaluateFitness(const Chromosome &chromo) {
    double totalProfit = 0.0;

    // Track used weight and volume for each compartment
    vector<double> usedWeight(NUM_COMPARTMENTS, 0.0);
    vector<double> usedVolume(NUM_COMPARTMENTS, 0.0);

    for (int c = 0; c < NUM_CARGOS; c++) {
        for (int k = 0; k < NUM_COMPARTMENTS; k++) {
            double tons = chromo.genes[c * NUM_COMPARTMENTS + k];
            if (tons < 0) return -1e9; // invalid solution

            usedWeight[k] += tons;
            usedVolume[k] += tons * cargos[c].volume / cargos[c].weight;
            totalProfit += tons * cargos[c].profit;
        }
    }

    // Check constraints (capacity)
    for (int k = 0; k < NUM_COMPARTMENTS; k++) {
        if (usedWeight[k] > compartments[k].maxWeight + 1e-6) return -1e9;
        if (usedVolume[k] > compartments[k].maxVolume + 1e-6) return -1e9;
    }

    // Balance constraint: proportionality of weight to volume capacity
    double totalCapVol = 0;
    for (int k = 0; k < NUM_COMPARTMENTS; k++) totalCapVol += compartments[k].maxVolume;

    double totalWeight = accumulate(usedWeight.begin(), usedWeight.end(), 0.0);
    for (int k = 0; k < NUM_COMPARTMENTS; k++) {
        double expectedRatio = compartments[k].maxVolume / totalCapVol;
        double actualRatio = (totalWeight > 0) ? (usedWeight[k] / totalWeight) : 0;
        if (fabs(expectedRatio - actualRatio) > 0.15) {
            return -1e9; // penalize imbalance
        }
    }

    return totalProfit;
}

// ============================
// Generate random chromosome
// ============================
Chromosome randomChromosome() {
    Chromosome chromo;
    chromo.genes.resize(NUM_CARGOS * NUM_COMPARTMENTS);

    for (int i = 0; i < NUM_CARGOS; i++) {
        double remaining = cargos[i].weight;
        for (int k = 0; k < NUM_COMPARTMENTS; k++) {
            double part = ((double) rand() / RAND_MAX) * remaining;
            chromo.genes[i * NUM_COMPARTMENTS + k] = part;
            remaining -= part;
            if (remaining < 0) remaining = 0;
        }
    }

    chromo.fitness = evaluateFitness(chromo);
    return chromo;
}

// ============================
// Selection: Tournament
// ============================
Chromosome tournamentSelection(const vector<Chromosome> &population) {
    int a = rand() % POP_SIZE;
    int b = rand() % POP_SIZE;
    return (population[a].fitness > population[b].fitness) ? population[a] : population[b];
}

// ============================
// Crossover: Single point
// ============================
Chromosome crossover(const Chromosome &p1, const Chromosome &p2) {
    Chromosome child;
    child.genes = p1.genes;

    if (((double) rand() / RAND_MAX) < CROSSOVER_RATE) {
        int point = rand() % child.genes.size();
        for (int i = point; i < (int) child.genes.size(); i++) {
            child.genes[i] = p2.genes[i];
        }
    }

    child.fitness = evaluateFitness(child);
    return child;
}

// ============================
// Mutation: Random adjustment
// ============================
void mutate(Chromosome &chromo) {
    for (double &g : chromo.genes) {
        if (((double) rand() / RAND_MAX) < MUTATION_RATE) {
            g += ((double) rand() / RAND_MAX - 0.5);
            if (g < 0) g = 0;
        }
    }
    chromo.fitness = evaluateFitness(chromo);
}

// ============================
// Main Genetic Algorithm
// ============================
int main() {
    srand(time(NULL));

    // Initialize population
    vector<Chromosome> population;
    for (int i = 0; i < POP_SIZE; i++) {
        population.push_back(randomChromosome());
    }

    Chromosome best = population[0];

    for (int gen = 0; gen < GENERATIONS; gen++) {
        vector<Chromosome> newPop;

        for (int i = 0; i < POP_SIZE; i++) {
            Chromosome p1 = tournamentSelection(population);
            Chromosome p2 = tournamentSelection(population);
            Chromosome child = crossover(p1, p2);
            mutate(child);
            newPop.push_back(child);
            if (child.fitness > best.fitness) best = child;
        }

        population = newPop;

        if (gen % 50 == 0) {
            cout << "Generation " << gen << " Best fitness: " << best.fitness << "\n";
        }
    }

    // Output best solution
    cout << "\nBest solution found:\n";
    for (int i = 0; i < NUM_CARGOS; i++) {
        cout << "Cargo C" << (i+1) << ": ";
        for (int k = 0; k < NUM_COMPARTMENTS; k++) {
            cout << fixed << setprecision(2) << best.genes[i * NUM_COMPARTMENTS + k] << "t in Compartment " << k+1 << "; ";
        }
        cout << "\n";
    }
    cout << "Total Profit: R$" << best.fitness << "\n";

    return 0;
}
