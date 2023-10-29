import matplotlib.pyplot as plt


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


def draw_path(points, edges, plot_title, output_file_name):
    plt.clf()

    points_x = [point[0] for point in points[1:]]
    points_y = [point[1] for point in points[1:]]
    plt.scatter(points_x, points_y, [15 for _ in range(len(points_x))], color="blue", marker='x')

    for edge in edges:
        xs = [points[edge[0]][0], points[edge[1]][0]]
        ys = [points[edge[0]][1], points[edge[1]][1]]
        plt.plot(xs, ys, color="green")

    plt.title(plot_title)
    plt.axis('off')
    plt.savefig(output_file_name, bbox_inches='tight')


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

    plt.scatter(reps10, min_series_10, marker='x', color="blue")
    plt.scatter(reps50, min_series_50, marker='x', color="green")

    plt.axhline(y = avg_series_10, color = 'blue', linestyle = '-', label="avg of min of 10")
    plt.axhline(y = avg_series_50, color = 'green', linestyle = '-', label="avg of min of 50")
    plt.axhline(y = min_weight, color = 'red', linestyle = '-', label="minimum")

    plt.legend(bbox_to_anchor=(1.05, 1.0), loc='upper left')
    plt.title(plot_title)
    plt.xlabel("cycle number")
    plt.ylabel("cycle total weight")
    plt.savefig(output_file_name, bbox_inches='tight')


TESTCASES = [
    "xqf131", "xqg237", "pma343", "pka379", "bcl380",
    "pbl395", "pbk411", "pbn423", "pbm436", "xql662"
]


for testcase in TESTCASES:
    points = read_points(testcase)
    mst_weigth, mst_edges = read_mst(testcase)
    tsp_cycle_weight, tsp_cycle_edges = read_tsp_cycle(testcase)
    random_cycle_weights = read_random_cycle_weights(testcase)

    # draw_path(points, mst_edges,
    #     "MST for {}; total weight = {}".format(testcase, mst_weigth), "plots/{}-mst.png".format(testcase))

    # draw_path(points, tsp_cycle_edges,
    #     "TSP cycle based on MST for {}; total weight = {}".format(testcase, tsp_cycle_weight), "plots/{}-tsp-mst.png".format(testcase))

    draw_random_cycle_stats(random_cycle_weights, "Random cycles weight for {}".format(testcase), "plots/{}-rand.png".format(testcase))
