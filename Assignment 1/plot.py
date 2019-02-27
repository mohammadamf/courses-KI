import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn

#each game is a row
#each row is 2 numbers (numb_moves, the_move) followed by 3 same length arrays (score, distancewQ, distance Wk)

def make_plots(fname, plotname):
    games_by_end = {"wins": [], "stalemates": [], "simple ties": []}
    n_moves_by_end = {"wins": [], "stalemates": [], "simple ties": []}
    with open(fname) as f:
        for line in f:
            raw_game = np.fromstring(line, sep=",");

            numb_moves = raw_game[0]
            the_move = raw_game[1]

            array_len = int((len(raw_game)-2)/3)
            game = {
                "score" :      raw_game[2+0*array_len:2+1*array_len],
                "distanceWQ" : raw_game[2+1*array_len:2+2*array_len],
                "distanceWK" : raw_game[2+2*array_len:2+3*array_len],
            }

            #fill games_by_end
            if the_move == 0:
                games_by_end["wins"].append(game)
                n_moves_by_end["wins"].append(numb_moves)
            elif the_move == 1:
                games_by_end["stalemates"].append(game)
                n_moves_by_end["stalemates"].append(numb_moves)
            else: #simple tie
                games_by_end["simple ties"].append(game)
                n_moves_by_end["simple ties"].append(numb_moves)

    fig = plt.figure()  # create a figure object
    ax = fig.add_subplot(1, 1, 1)  # create an axes object in the figure
    ax.set_title(plotname)
    for game in games_by_end["wins"]:
        print(game["score"].shape)
        print(np.arange(0,len(game["score"])))
        ax.scatter(np.arange(0,len(game["score"])),game["score"], color="green", label="wins")
    for game in games_by_end["stalemates"]:
        ax.scatter(np.arange(0,len(game["score"])),game["score"], color="red", label="stalemates")
    for game in games_by_end["simple ties"]:
        ax.scatter(np.arange(0,len(game["score"])),game["score"], color="blue", label="simple ties")
    fig.savefig(plotname+'score.png', bbox_inches='tight')

    fig = plt.figure()  # create a figure object
    ax = fig.add_subplot(1, 1, 1)  # create an axes object in the figure
    ax.set_title(plotname)
    for game in games_by_end["wins"]:
        ax.scatter(np.arange(0,len(game["distanceWQ"])), game["distanceWQ"], color="green", label="wins")
        ax.scatter(np.arange(0,len(game["distanceWK"])),game["distanceWK"], color="green", label="wins")
    for game in games_by_end["stalemates"]:
        ax.scatter(np.arange(0,len(game["distanceWQ"])),game["distanceWQ"], color="red", label="stalemates")
        ax.scatter(np.arange(0,len(game["distanceWK"])),game["distanceWK"], color="red", label="stalemates")
    for game in games_by_end["simple ties"]:
        ax.scatter(np.arange(0,len(game["distanceWQ"])),game["distanceWQ"], color="blue", label="simple ties")
        ax.scatter(np.arange(0,len(game["distanceWK"])),game["distanceWK"], color="blue", label="simple ties")
    fig.savefig(plotname+'distance.png', bbox_inches='tight')



    fig = plt.figure()  # create a figure object
    ax.set_title(plotname)
    ax = fig.add_subplot(1, 1, 1)  # create an axes object in the figure
    ax.hist(n_moves_by_end["wins"], 50, density=1, facecolor='green', alpha=0.3, label="wins")
    ax.hist(n_moves_by_end["stalemates"], 50, density=1, facecolor='red', alpha=0.3, label="stalemates")
    ax.hist(n_moves_by_end["simple ties"], 50, density=1, facecolor='blue', alpha=0.2, label="simple ties")
    plt.legend()
    fig.savefig(plotname+'n_moves.png', bbox_inches='tight')

make_plots("random_stats.csv", "willikeurige agent")
make_plots("simple_stats.csv", "simpele agent")
make_plots("monte_carlo_stats.csv", "monte carlo agent")
