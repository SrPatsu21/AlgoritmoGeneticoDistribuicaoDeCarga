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
const int ELITISM = 4;

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
const int POP_SIZE = 1000;
const int GENERATIONS = 500;
double MUTATION_RATE = 0.4;
double CROSSOVER_RATE = 0.8;
const double IGNORE_POP_PERCENT = 0.05;
const double REDUCE_RATE = 0.01;
const int STAGNANT_TIMES = 10;
const double MIN_MUTATION_RATE = 0.01;
const double MIN_CROSSOVER_RATE = 0.05;

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
    std::vector<double> totalCargoUsed(NUM_CARGOS, 0.0);

    for (int n_cargos = 0; n_cargos < NUM_CARGOS; n_cargos++) {
        for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) {
            double tons = chromo.genes[n_cargos * NUM_COMPARTMENTS + n_partment];
            if (tons < 0) return -1e9; // invalid solution

            totalCargoUsed[n_cargos] += tons;
            usedWeight[n_partment] += tons;
            usedVolume[n_partment] += tons * cargos[n_cargos].volume / cargos[n_cargos].weight;
            totalProfit += tons * cargos[n_cargos].profit;
        }

        if (totalCargoUsed[n_cargos] > cargos[n_cargos].weight + 1e-6) {
            return -1e9;
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

    // Proporcionalidade de peso para volume (opcional)
    double totalCapVol = 0;
    for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) totalCapVol += compartments[n_partment].maxVolume;

    double totalWeight = accumulate(usedWeight.begin(), usedWeight.end(), 0.0);
    for (int n_partment = 0; n_partment < NUM_COMPARTMENTS; n_partment++) {
        double expectedRatio = compartments[n_partment].maxVolume / totalCapVol;
        double actualRatio = (totalWeight > 0) ? (usedWeight[n_partment] / totalWeight) : 0;
        if (fabs(expectedRatio - actualRatio) > 0.15) return -1e9;
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
    int c = rand() % POP_SIZE;
    int d = rand() % POP_SIZE;
    if (population[a].fitness > population[b].fitness)
    {
        if (population[c].fitness > population[d].fitness)
        {
            return (population[a].fitness > population[c].fitness) ? population[a] : population[c];
        }else
        {
            return (population[a].fitness > population[d].fitness) ? population[a] : population[d];
        }
    }else{
        if (population[c].fitness > population[d].fitness)
        {
            return (population[b].fitness > population[c].fitness) ? population[b] : population[c];
        }else
        {
            return (population[b].fitness > population[d].fitness) ? population[b] : population[d];
        }
    }
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
    int stagnant = 0;
    double last_best = 0.0;
    int crossover_total = POP_SIZE-(POP_SIZE*IGNORE_POP_PERCENT);

    for (int gen = 0; gen < GENERATIONS; gen++) {
        std::vector<Chromosome> newPop;
        for (size_t i = 0; i < ELITISM; i++)
        {
            newPop.push_back(population[i]);
            if (population[i].fitness > best.fitness) {
                best = population[i];
            }
        }

        for (int i = newPop.size(); i < crossover_total; i++) {
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
        if (MUTATION_RATE > MIN_MUTATION_RATE || CROSSOVER_RATE > MIN_CROSSOVER_RATE)
        {
            if (last_best == best.fitness && best.fitness != -1e9)
            {
                stagnant++;
                if (stagnant >= STAGNANT_TIMES)
                {
                    if (MUTATION_RATE > MIN_MUTATION_RATE)
                    {
                        MUTATION_RATE = MUTATION_RATE-(MUTATION_RATE * REDUCE_RATE);
                    }
                    if (CROSSOVER_RATE > MIN_CROSSOVER_RATE)
                    {
                        CROSSOVER_RATE = CROSSOVER_RATE-(CROSSOVER_RATE * REDUCE_RATE);
                    }
                }
            }else{
                stagnant = 0;
            }
            last_best = best.fitness;
        }

        if (gen % 50 == 0) {
            std::cout << "Generation " << gen << " Best fitness: " << best.fitness << ", mutation:" << MUTATION_RATE << ", crossover:" << CROSSOVER_RATE << "\n";
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
