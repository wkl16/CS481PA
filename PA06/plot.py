import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

matplotlib.rcParams['font.family'] = 'serif'
matplotlib.rcParams['font.size'] = 12
ns = list(range(2, 7, 2))
m = 7

data_sets = [
        {'n': n,
         'm': m,
         'data': pd.read_csv(f'{n}_{m}.csv', index_col=0)
         } for n in ns]

title_fontsize = 14
label_fontsize = 12
legend_fontsize = 10
ticks_fontsize = 10

colors = ['#E69F00', '#56B4E9', '#009E73']
# labels = ["CARNUM = 2\nMAXPERCAR = 7", "CARNUM = 4\nMAXPERCAR = 7", "CARNUM = 6\nMAXPERCAR = 7"]

def plot_arrive(data):
    plt.figure(figsize=(10, 6))
    plt.plot(data.index, data.arrive, linestyle='-', color=colors[0], linewidth=2, marker='o', markersize=4)
    plt.title("Number of Persons Arrived Per Minute", fontsize=title_fontsize)
    plt.xlabel("Time (minutes)", fontsize=label_fontsize)
    plt.ylabel("Number of Arrivals", fontsize=label_fontsize)
    plt.grid(visible=True, linestyle='--', alpha=0.7)
    plt.xticks(fontsize=ticks_fontsize)
    plt.yticks(fontsize=ticks_fontsize)
    plt.tight_layout()
    # plt.savefig("arrivals_plot.png")
    plt.savefig("arrivals_plotV1.eps", format="eps", dpi=300)
    # plt.show()

def plot_reject(data_sets):
    plt.figure(figsize=(10, 6))

    for i, data_set in enumerate(data_sets):
        data = data_set['data']
        label = f'car={data_set['n']},person={data_set['m']}'
        plt.plot(data.index, data.reject, label=label, color=colors[i % len(colors)], 
                 linestyle='-', linewidth=2, marker='o', markersize=4)

    plt.title("Number of Persons Rejected Per Minute", fontsize=title_fontsize)
    plt.xlabel("Time (minutes)", fontsize=label_fontsize)
    plt.ylabel("Number of Rejections", fontsize=label_fontsize)
    plt.grid(visible=True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=legend_fontsize, title_fontsize=label_fontsize)
    plt.xticks(fontsize=ticks_fontsize)
    plt.yticks(fontsize=ticks_fontsize)
    plt.tight_layout()
    # plt.savefig("rejected_plot.png")
    plt.savefig("rejected_plotV1.eps", format="eps", dpi=300)
    # plt.show()


def plot_wait(data_sets):
    plt.figure(figsize=(10, 6))

    for i, data_set in enumerate(data_sets):
        data = data_set['data']
        label = f'car={data_set['n']},person={data_set['m']}'
        plt.plot(data.index, data.wait, label=label, color=colors[i % len(colors)], 
                 linestyle='-', linewidth=2)

    plt.title("Number of Persons Waiting Per Minute", fontsize=title_fontsize)
    plt.xlabel("Time (minutes)", fontsize=label_fontsize)
    plt.ylabel("Number of Waiting Passengers", fontsize=label_fontsize)
    plt.grid(visible=True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=legend_fontsize, title_fontsize=label_fontsize)
    plt.xticks(fontsize=ticks_fontsize)
    plt.yticks(fontsize=ticks_fontsize)
    plt.tight_layout()
    # plt.savefig("waiting_plot.png")
    plt.savefig("waiting_plotV1.eps", format="eps", dpi=300)
    # plt.show()


plot_arrive(data_sets[0]['data'])
plot_reject(data_sets)
plot_wait(data_sets)


plt.show()
