import matplotlib.pyplot as plt
import pandas as pd

ns = list(range(2, 7, 2))
m = 7

data_sets = [
        {'n': n,
         'm': m,
         'data': pd.read_csv(f'{n}_{m}.csv', index_col=0)
         } for n in ns]


def plot_arrive(data):
    plt.figure()
    plt.plot(data.index, data.arrive, 'r.-')
    plt.xlabel("Time (minutes)")
    plt.ylabel("Number of People Arrived")
    plt.title("Arrivals")
    plt.grid()
    plt.xlim([0, 600])
    plt.ylim([0, 80])


def plot_reject(data_sets):
    plt.figure()

    for data_set in data_sets:
        data = data_set['data']
        label = f'car={data_set['n']},person={data_set['m']}'
        plt.plot(data.index, data.reject, '.-', label=label)

    plt.xlabel('Time (minutes)')
    plt.ylabel('Number of People Rejected')
    plt.title('Rejections')
    plt.grid()
    plt.xlim([0, 600])
    plt.ylim([0, 80])
    plt.legend()


def plot_wait(data_sets):
    plt.figure()

    for data_set in data_sets:
        data = data_set['data']
        label = f'car={data_set['n']},person={data_set['m']}'
        plt.plot(data.index, data.wait, '.-', label=label)

    plt.xlabel('Time (minutes)')
    plt.ylabel('Number of People Waiting')
    plt.title('Waiting Queue')
    plt.grid()
    plt.xlim([0, 600])
    plt.ylim([0, 1000])
    plt.legend()


plot_arrive(data_sets[0]['data'])
plot_reject(data_sets)
plot_wait(data_sets)


plt.show()
