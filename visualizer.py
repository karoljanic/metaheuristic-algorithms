import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def read_points(testcase_name):
    with open("examples/{}.tsp".format(testcase_name), "r") as file:
        lines = [line.strip() for line in file.readlines()]
        proper_lines = lines[lines.index("NODE_COORD_SECTION") + 1:lines.index("EOF")]
        proper_lines = [line.split() for line in proper_lines]

        points = [[0, 0]]
        for line in proper_lines:
            points.append([int(line[1]), int(line[2])])

        return points


def read_mst(testcase_name):
    with open("results/{}-mst.txt".format(testcase_name), "r") as file:
        lines = [line.strip() for line in file.readlines()]
        proper_lines = [line.split() for line in lines[2:] if line != ""]

        weight = int(lines[0].split()[1])
        edges = []
        for line in proper_lines:
            edges.append([int(line[0]), int(line[1])])

        return weight, edges


def read_tsp_cycle(testcase_name):
    with open("results/{}-tsp-mst.txt".format(testcase_name), "r") as file:
        lines = [line.strip() for line in file.readlines()]
        proper_lines = [line.split()[0] for line in lines[2:] if line != ""]

        weight = int(lines[0].split()[1])
        edges = []
        n = len(proper_lines)
        for line_index in range(n):
            edges.append([int(proper_lines[line_index]), int(proper_lines[(line_index + 1) % n])])

        return weight, edges


def read_random_cycle_weights(testcase_name):
    with open("results/{}-rand.txt".format(testcase_name), "r") as file:
        cycle_weights = [int(line.strip()) for line in file.readlines()]

        return cycle_weights


def read_local_search_results(testcase_name, method_name):
    with open("results/{}-{}.txt".format(testcase_name, method_name), "r") as file:
        lines = [line.strip().split() for line in file.readlines()]
        improvements = []
        initial_cycle_weights = []
        final_cycle_weights = []
        cycles = []

        cycle = []
        for line in lines:
            if len(line) == 2 and line[0] == "improvements:":
                improvements.append(int(line[1]))
            elif len(line) == 2 and line[0] == "initial-cycle-weight:":
                initial_cycle_weights.append(int(line[1]))
            elif len(line) == 2 and line[0] == "final-cycle-weight:":
                final_cycle_weights.append(int(line[1]))
            elif len(line) == 1:
                cycle.append(int(line[0]))
            elif len(line) == 0:
                cycles.append(cycle)
                cycle = []

        return improvements, initial_cycle_weights, final_cycle_weights, cycles


def read_sa_tuning_results(testcase_name, tuning_param):
    with open("results/{}-sa-tuning-{}.txt".format(testcase_name, tuning_param), "r") as file:
        lines = [line.strip().split() for line in file.readlines()]
        alpha = []
        beta = []
        gamma = []
        delta = []
        min_result = []
        avg_result = []
        time_per_rep = []

        for line in lines:
            alpha.append(float(line[0]))
            beta.append(float(line[1]))
            gamma.append(float(line[2]))
            delta.append(float(line[3]))
            min_result.append(float(line[4]))
            avg_result.append(float(line[5]))
            time_per_rep.append(float(line[6]))

        return alpha, beta, gamma, delta, min_result, avg_result, time_per_rep
    

def read_ts_tuning_results(testcase_name, tuning_param):
    with open("results/{}-ts-tuning-{}.txt".format(testcase_name, tuning_param), "r") as file:
        lines = [line.strip().split() for line in file.readlines()]
        alpha = []
        beta = []
        min_result = []
        avg_result = []
        time_per_rep = []

        for line in lines:
            alpha.append(float(line[0]))
            beta.append(float(line[1]))
            min_result.append(float(line[2]))
            avg_result.append(float(line[3]))
            time_per_rep.append(float(line[4]))

        return alpha, beta, min_result, avg_result, time_per_rep


def draw_simple_plot(x, ys, labels, colors, x_label, y_label, plot_averages, plot_title, output_file_name):
    plt.clf()

    for y, label, color in zip(ys, labels, colors):
        plt.scatter(x, y, label=label, color=color, marker="x")

    if plot_averages:
        avgs = [sum(y) / len(y) for y in ys]

        for avg, label, color in zip(avgs, labels, colors):
            plt.axhline(y=avg, label="{}-average({})".format(label, int(avg)), color=color, linestyle="-")

    plt.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    plt.title(plot_title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.savefig(output_file_name, bbox_inches="tight")


def draw_path(points, edges, plot_title, output_file_name):
    plt.clf()

    points_x = [point[0] for point in points[1:]]
    points_y = [point[1] for point in points[1:]]
    plt.scatter(points_x, points_y, [15 for _ in range(len(points_x))], color="blue", marker="x")

    for edge in edges:
        xs = [points[edge[0]][0], points[edge[1]][0]]
        ys = [points[edge[0]][1], points[edge[1]][1]]
        plt.plot(xs, ys, color="green")

    plt.title(plot_title)
    plt.axis("off")
    plt.savefig(output_file_name, bbox_inches="tight")


def draw_random_cycle_stats(cycle_weights, plot_title, output_file_name):
    plt.clf()

    series10 = [cycle_weights[i:i + 10] for i in range(0, len(cycle_weights), 10)]
    series50 = [cycle_weights[i:i + 50] for i in range(0, len(cycle_weights), 50)]

    min_series_10 = [min(serie) for serie in series10]
    min_series_50 = [min(serie) for serie in series50]

    avg_series_10 = sum(min_series_10) / len(min_series_10)
    avg_series_50 = sum(min_series_50) / len(min_series_50)

    min_weight = min(cycle_weights)

    print("{}: min={}  avg10={}  avg50={}".format(plot_title, min_weight, avg_series_10, avg_series_50))

    reps10 = [rep for rep in range(10, len(cycle_weights) + 1, 10)]
    reps50 = [rep for rep in range(50, len(cycle_weights) + 1, 50)]

    plt.scatter(reps10, min_series_10, marker="x", color="blue")
    plt.scatter(reps50, min_series_50, marker="x", color="green")

    plt.axhline(y=avg_series_10, color="blue", linestyle="-", label="avg of min of 10")
    plt.axhline(y=avg_series_50, color="green", linestyle="-", label="avg of min of 50")
    plt.axhline(y=min_weight, color="red", linestyle="-", label="minimum")

    plt.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    plt.title(plot_title)
    plt.xlabel("cycle number")
    plt.ylabel("cycle total weight")
    plt.savefig(output_file_name, bbox_inches="tight")


def draw_local_search_stats(improvements, initial_weights, final_weights, plot_title, output_file_name):
    plt.clf()

    repetitions = list(range(len(improvements)))
    improvements_avg = sum(improvements) / len(improvements)
    final_weights_avg = sum(final_weights) / len(final_weights)
    best_final_weight = min(final_weights)

    plt.scatter(repetitions, initial_weights, marker="x", color="green", label="initial cycle")
    plt.scatter(repetitions, final_weights, marker="x", color="blue", label="final cycle")
    plt.axhline(y=final_weights_avg, color="magenta", linestyle="-",
                label="average final weights sum\n({})".format(int(final_weights_avg)))
    plt.axhline(y=best_final_weight, color="red", linestyle="-",
                label="best final weights sum\n({})".format(int(best_final_weight)))

    plt.legend(bbox_to_anchor=(1.05, 1.0), loc="upper left")
    plt.title(plot_title)
    plt.xlabel("repetition number")
    plt.ylabel("cycle total weight")
    plt.savefig(output_file_name, bbox_inches="tight")


def process_local_search(testcase_name, points, improvements, initial_weights, final_weights, cycles, method_name,
                         filename_sufix):
    best_cycle_index = final_weights.index(min(final_weights))
    best_cycle_improvements = improvements[best_cycle_index]
    best_cycle_initial_weight = initial_weights[best_cycle_index]
    best_cycle_final_weight = final_weights[best_cycle_index]
    cycle_average_weight = sum(final_weights) / len(final_weights)
    cycle_average_improvements = sum(improvements) / len(improvements)
    best_cycle = cycles[best_cycle_index]

    cycle_edges = []
    for index in range(len(best_cycle)):
        cycle_edges.append([best_cycle[index], best_cycle[(index + 1) % len(best_cycle)]])

    print("TSP cycle for {} and {}: best={}  avg={}  imp={}".format(testcase_name, method_name, best_cycle_final_weight,
                                                                    cycle_average_weight, cycle_average_improvements))

    draw_path(points, cycle_edges,
              "TSP cycle for {};\n{}; total weight = {}".format(testcase_name, method_name, best_cycle_final_weight),
              "plots/{}-{}.png".format(testcase, filename_sufix))

    draw_local_search_stats(improvements, initial_weights, final_weights,
                            "TSP cycle for {};\n{}".format(testcase_name, method_name),
                            "plots/{}-{}-stats.png".format(testcase, filename_sufix))
    

def analyze_sa_tuning(testcases):
    alpha_changes = []
    min_changes = []
    avg_changes = []
    time_changes = []
    for testcase in testcases:
        alpha, _, _, _, min_result, avg_result, time_per_rep = read_sa_tuning_results(testcase, "alpha")
        alpha_changes.append(alpha)
        min_changes.append(min_result)
        avg_changes.append(avg_result)
        time_changes.append(time_per_rep)

    plt.clf()
    for i in range(len(testcases)):
        plt.plot(alpha_changes[i], min_changes[i], label=testcases[i])
    plt.xlabel("alpha")
    plt.ylabel("minimalna waga rozwiązania")
    plt.suptitle(r'Wpływ parametru alpha na najlepszy wynik metaheurystyki')
    plt.title(r'$\beta = 0.99$, $\gamma = 0.3$, $\delta = 0.3$')
    plt.savefig("plots/sa-tuning-alpha-min.png", bbox_inches="tight")

    plt.clf()
    for i in range(len(testcases)):
        plt.plot(alpha_changes[i], avg_changes[i], label=testcases[i])
    plt.xlabel("alpha")
    plt.ylabel("średnia waga rozwiązania")
    plt.suptitle(r"Wpływ parametru alpha na średni wynik metaheurystyki")
    plt.title(r'$\beta = 0.99$, $\gamma = 0.3$, $\delta = 0.3$')
    plt.savefig("plots/sa-tuning-alpha-avg.png", bbox_inches="tight")

    plt.clf()
    for i in range(len(testcases)):
        plt.plot(alpha_changes[i], time_changes[i], label=testcases[i])
    plt.xlabel("alpha")
    plt.ylabel("średni czas działania")
    plt.suptitle(r"Wpływ parametru alpha na czas działania metaheurystyki")
    plt.title(r'$\beta = 0.99$, $\gamma = 0.3$, $\delta = 0.3$')
    plt.savefig("plots/sa-tuning-alpha-time.png", bbox_inches="tight")


    beta_changes = []
    min_changes = []
    avg_changes = []
    time_changes = []
    for testcase in testcases:
        _, beta, _, _, min_result, avg_result, time_per_rep = read_sa_tuning_results(testcase, "beta")
        beta_changes.append(beta)
        min_changes.append(min_result)
        avg_changes.append(avg_result)
        time_changes.append(time_per_rep)

    plt.clf()
    for i in range(len(testcases)):
        plt.plot(beta_changes[i], min_changes[i], label=testcases[i])
    plt.xlabel("beta")
    plt.ylabel("minimalna waga rozwiązania")
    plt.suptitle(r"Wpływ parametru beta na najlepszy wynik metaheurystyki")
    plt.title(r"$\alpha = 0.5$, $\gamma = 0.3$, $\delta = 0.3$")
    plt.savefig("plots/sa-tuning-beta-min.png", bbox_inches="tight")

    plt.clf()
    for i in range(len(testcases)):
        plt.plot(beta_changes[i], avg_changes[i], label=testcases[i])
    plt.xlabel("beta")
    plt.ylabel("średnia waga rozwiązania")
    plt.suptitle(r"Wpływ parametru beta na średni wynik metaheurystyki")
    plt.title(r"$\alpha = 0.5$, $\gamma = 0.3$, $\delta = 0.3$")
    plt.savefig("plots/sa-tuning-beta-avg.png", bbox_inches="tight")

    plt.clf()
    for i in range(len(testcases)):
        plt.plot(beta_changes[i], time_changes[i], label=testcases[i])
    plt.xlabel("beta")
    plt.ylabel("średni czas działania")
    plt.suptitle(r"Wpływ parametru beta na czas działania metaheurystyki")
    plt.title(r"$\alpha = 0.5$, $\gamma = 0.3$, $\delta = 0.3$")
    plt.savefig("plots/sa-tuning-beta-time.png", bbox_inches="tight")


    for testcase in testcases:
        _, _, gamma, delta, min_result, avg_result, time_per_rep = read_sa_tuning_results(testcase, "gamma-delta")
        min_data = []
        avg_data = []
        time_data = []
        for i in range(len(gamma)):
            min_data.append([gamma[i], delta[i], min_result[i]])
            avg_data.append([gamma[i], delta[i], avg_result[i]])
            time_data.append([gamma[i], delta[i], time_per_rep[i]])

        min_df = pd.DataFrame(min_data, columns=['gamma', 'delta', 'minimalna waga cyklu'])
        avg_df = pd.DataFrame(avg_data, columns=['gamma', 'delta', 'średnia waga cyklu'])
        time_df = pd.DataFrame(time_data, columns=['gamma', 'delta', 'średni czas działania'])

        min_df_heatmap_data = min_df.pivot('gamma', 'delta', 'minimalna waga cyklu')
        avg_df_heatmap_data = avg_df.pivot('gamma', 'delta', 'średnia waga cyklu')
        time_df_heatmap_data = time_df.pivot('gamma', 'delta', 'średni czas działania')

        plt.clf()
        sns.heatmap(min_df_heatmap_data, cmap='viridis', annot=True, fmt='.2f', linewidths=.5)
        plt.suptitle(r"Wpływ parametrów gamma i delta na najlepszy wynik metaheurystyki")
        plt.title(r"$\alpha = 0.5$, $\beta = 0.9$")
        plt.savefig("plots/sa-tuning-gamma-delta-min-{}.png".format(testcase), bbox_inches="tight")

        plt.clf()
        sns.heatmap(avg_df_heatmap_data, cmap='viridis', annot=True, fmt='.2f', linewidths=.5)
        plt.suptitle(r"Wpływ parametrów gamma i delta na średni wynik metaheurystyki")
        plt.title(r"$\alpha = 0.5$, $\beta = 0.9$")
        plt.savefig("plots/sa-tuning-gamma-delta-avg-{}.png".format(testcase), bbox_inches="tight")

        plt.clf()
        sns.heatmap(time_df_heatmap_data, cmap='viridis', annot=True, fmt='.2f', linewidths=.5)
        plt.suptitle(r"Wpływ parametrów gamma i delta na średni czas działania metaheurystyki")
        plt.title(r"$\alpha = 0.5$, $\beta = 0.9$")
        plt.savefig("plots/sa-tuning-gamma-delta-time-{}.png".format(testcase), bbox_inches="tight")

    
def analyze_ts_tuning(testcases):
    for testcase in testcases:
        alpha, beta, min_result, avg_result, time_per_rep = read_ts_tuning_results(testcase, "alpha-beta")
        min_data = []
        avg_data = []
        time_data = []

        for i in range(len(alpha)):
            min_data.append([alpha[i], beta[i], min_result[i]])
            avg_data.append([alpha[i], beta[i], avg_result[i]])
            time_data.append([alpha[i], beta[i], time_per_rep[i]])

        min_df = pd.DataFrame(min_data, columns=['alpha', 'beta', 'minimalna waga cyklu'])
        avg_df = pd.DataFrame(avg_data, columns=['alpha', 'beta', 'średnia waga cyklu'])
        time_df = pd.DataFrame(time_data, columns=['alpha', 'beta', 'średni czas działania'])

        min_df_heatmap_data = min_df.pivot('alpha', 'beta', 'minimalna waga cyklu')
        avg_df_heatmap_data = avg_df.pivot('alpha', 'beta', 'średnia waga cyklu')
        time_df_heatmap_data = time_df.pivot('alpha', 'beta', 'średni czas działania')

        plt.clf()
        sns.heatmap(min_df_heatmap_data, cmap='viridis', annot=True, fmt='.2f', linewidths=.5)
        plt.title(r"Wpływ parametrów alpha i beta na najlepszy wynik metaheurystyki")
        plt.savefig("plots/ts-tuning-alpha-beta-min-{}.png".format(testcase), bbox_inches="tight")

        plt.clf()
        sns.heatmap(avg_df_heatmap_data, cmap='viridis', annot=True, fmt='.2f', linewidths=.5)
        plt.title(r"Wpływ parametrów alpha i beta na średni wynik metaheurystyki")
        plt.savefig("plots/ts-tuning-alpha-beta-avg-{}.png".format(testcase), bbox_inches="tight")

        plt.clf()
        sns.heatmap(time_df_heatmap_data, cmap='viridis', annot=True, fmt='.2f', linewidths=.5)
        plt.title(r"Wpływ parametrów alpha i beta na średni czas działania metaheurystyki")
        plt.savefig("plots/ts-tuning-alpha-beta-time-{}.png".format(testcase), bbox_inches="tight")


TESTCASES = [
    "xqf131", "xqg237", "pma343", "pka379", "bcl380",
    "pbl395", "pbk411", "pbn423", "pbm436", "xql662",
    "xit1083", "icw1483", "djc1785", "dcb2086", "pds2566"
]

for testcase in TESTCASES:
    points = read_points(testcase)

    # mst_weigth, mst_edges = read_mst(testcase)
    # tsp_cycle_weight, tsp_cycle_edges = read_tsp_cycle(testcase)
    # random_cycle_weights = read_random_cycle_weights(testcase)
    # ls1_imp, ls1_iw, ls1_fw, ls1_cyc = read_local_search_results(testcase, "local-search-mst")
    # ls2_imp, ls2_iw, ls2_fw, ls2_cyc = read_local_search_results(testcase, "local-search-rand")
    # ls3_imp, ls3_iw, ls3_fw, ls3_cyc = read_local_search_results(testcase, "local-search-rand-rand-neighbourhood")

    # draw_path(points, mst_edges,
    #           "MST for {}; total weight = {}".format(testcase, mst_weigth), "plots/{}-mst.png".format(testcase))

    # draw_path(points, tsp_cycle_edges,
    #           "TSP cycle based on MST for {}; total weight = {}".format(testcase, tsp_cycle_weight),
    #           "plots/{}-tsp-mst.png".format(testcase))

    # draw_random_cycle_stats(random_cycle_weights, "Random cycles weight for {}".format(testcase),
    #                         "plots/{}-rand.png".format(testcase))

    # print("TSP cycle based on MST for {}: {}".format(testcase, tsp_cycle_weight))

    # process_local_search(testcase, points, ls1_imp, ls1_iw, ls1_fw, ls1_cyc,
    #                      "Local Search with initial MST solution", "tsp-ls-mst")

    # process_local_search(testcase, points, ls2_imp, ls2_iw, ls2_fw, ls2_cyc,
    #                      "Local Search with initial random solution", "tsp-ls-rand")

    # process_local_search(testcase, points, ls3_imp, ls3_iw, ls3_fw, ls3_cyc,
    #                      "Local Search with initial random solution and random neighbourhood", "tsp-ls-rand-rand")

    # min_improvements_size = min(len(ls1_imp), len(ls2_imp), len(ls3_imp))
    # draw_simple_plot(list(range(min_improvements_size)),
    #                  [ls1_imp[:min_improvements_size], ls2_imp[:min_improvements_size],
    #                   ls3_imp[:min_improvements_size]],
    #                  ["mst initialization\nfull neighbourhood", "random initialization\nfull neighbourhood",
    #                   "random initialization\nrandom part of neighbourhood"],
    #                  ["blue", "red", "green"], "repetition number", "impovements number", True,
    #                  "Local Search improvements in {}".format(testcase), "plots/{}-ls-imp.png".format(testcase))

    # temperature, alpha, beta, gamma, min_result, avg_result, time_per_rep = read_sa_tuning_results(testcase)
    # analyze_sa_tuning(temperature, alpha, beta, gamma, min_result, avg_result, time_per_rep, testcase)
    
    # alpha, beta, min_result, avg_result, time_per_rep = read_ts_tuning_results(testcase)
    # analyze_ts_tuning(alpha, beta, min_result, avg_result, time_per_rep, testcase)

analyze_sa_tuning(TESTCASES[:2])
analyze_ts_tuning(TESTCASES[:2])