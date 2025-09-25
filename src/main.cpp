#include <bits/stdc++.h>

struct Cargo {
    double volume;
    double weight;
    double profit;
};

struct Compartment {
    double maxVolume;
    double maxWeight;
};

const int NUM_CARGOS = 4;
const int NUM_COMPARTMENTS = 3;
const int NUM_GENES = NUM_CARGOS * NUM_COMPARTMENTS;

Cargo cargos[NUM_CARGOS] = {
//  (m³/ton), (ton), (R$/ton)
    {480, 18, 310},
    {650, 15, 380},
    {580, 23, 350},
    {390, 12, 285}
};

Compartment compartments[NUM_COMPARTMENTS] = {
//  (m³), (ton)
    {6800, 10},   // Front
    {8700, 16},   // Center
    {5300, 8}     // Rear
};

// Genetic Algorithm parameters
const int POP_SIZE = 500;
const int GENERATIONS = 500;
const double MUTATION_RATE = 0.1;
const double CROSSOVER_RATE = 0.8;

/**
    Chromosome representation:
    A chromosome is a vector with NUM_CARGOS * NUM_COMPARTMENTS elements
    Each element = tons of cargo allocated to a given compartment
*/
struct Chromosome {
    std::vector<double> genes;
    double fitness;
};

double evaluateFitness(const Chromosome &chromo) {
    double totalProfit = 0.0;

    std::vector<double> usedWeight(NUM_COMPARTMENTS, 0.0);
    std::vector<double> usedVolume(NUM_COMPARTMENTS, 0.0);

    for (int n_cargos = 0; n_cargos < NUM_CARGOS; n_cargos++) {
        for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) {
            double tons = chromo.genes[n_cargos * NUM_COMPARTMENTS + n_partment];
            if (tons < 0) return -1e9; // invalid solution

            usedWeight[n_partment] += tons;
            usedVolume[n_partment] += tons * cargos[n_cargos].volume / cargos[n_cargos].weight;
            totalProfit += tons * cargos[n_cargos].profit;
        }
    }

    // Check constraints capacity
    for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) {
        if (usedWeight[n_partment] > compartments[n_partment].maxWeight) {
            return -1e9;
        }
        if (usedVolume[n_partment] > compartments[n_partment].maxVolume) {
            return -1e9;
        };
    }

    // proportionality of weight to volume capacity
    double totalCapVol = 0;
    for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) totalCapVol += compartments[n_partment].maxVolume;

    double totalWeight = accumulate(usedWeight.begin(), usedWeight.end(), 0.0);
    for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) {
        double expectedRatio = compartments[n_partment].maxVolume / totalCapVol;
        double actualRatio = (totalWeight > 0) ? (usedWeight[n_partment] / totalWeight) : 0;
        if (fabs(expectedRatio - actualRatio) > 0.15) {
            return -1e9;
        }
    }

    return totalProfit;
}

Chromosome randomChromosome() {
    Chromosome chromo;
    chromo.genes.resize(NUM_GENES);

    for (int i = 0; i < NUM_CARGOS; i++) {
        double remaining = cargos[i].weight;
        for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) {
            double part = ((double) rand() / RAND_MAX) * remaining;
            chromo.genes[i * NUM_COMPARTMENTS + n_partment] = part;
            remaining -= part;
            if (remaining < 0) remaining = 0;
        }
    }

    chromo.fitness = evaluateFitness(chromo);
    return chromo;
}

Chromosome tournamentSelection(const std::vector<Chromosome> &population) {
    int a = rand() % POP_SIZE;
    int b = rand() % POP_SIZE;
    return (population[a].fitness > population[b].fitness) ? population[a] : population[b];
}

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

void mutate(Chromosome &chromo) {
    for (double &g : chromo.genes) {
        if (((double) rand() / RAND_MAX) < MUTATION_RATE) {
            g += ((double) rand() / RAND_MAX - 0.5);
            if (g < 0) g = 0;
        }
    }
    chromo.fitness = evaluateFitness(chromo);
}

int main() {
    srand(time(NULL));

    // Initialize population
    std::vector<Chromosome> population;
    for (int i = 0; i < POP_SIZE; i++) {
        population.push_back(randomChromosome());
    }

    Chromosome best = population[0];

    for (int gen = 0; gen < GENERATIONS; gen++) {
        std::vector<Chromosome> newPop;

        for (int i = 0; i < POP_SIZE; i++) {
            Chromosome p1 = tournamentSelection(population);
            Chromosome p2 = tournamentSelection(population);
            Chromosome child = crossover(p1, p2);
            mutate(child);
            newPop.push_back(child);
            if (child.fitness > best.fitness) {
                best = child;
            }
        }
        for (size_t i = newPop.size(); i < POP_SIZE; i++)
        {
            Chromosome newChromosome = randomChromosome();
            newPop.push_back(newChromosome);
            if (newChromosome.fitness > best.fitness) {
                best = newChromosome;
            }
        }

        population = newPop;

        if (gen % 50 == 0) {
            std::cout << "Generation " << gen << " Best fitness: " << best.fitness << "\n";
        }
    }

    // Output best solution
    std::cout << "\nBest solution found:\n";
    for (int i = 0; i < NUM_CARGOS; i++) {
        std::cout << "Cargo C" << (i+1) << ": ";
        for (int k = 0; k < NUM_COMPARTMENTS; k++) {
            std::cout << std::fixed << std::setprecision(2) << best.genes[i * NUM_COMPARTMENTS + k] << "t in Compartment " << k+1 << "; ";
        }
        std::cout << "\n";
    }
    std::cout << "Total Profit: R$" << best.fitness << "\n";

    return 0;
}
