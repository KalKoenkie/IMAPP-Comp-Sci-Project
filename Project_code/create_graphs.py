import matplotlib.pyplot as plt

# Import the data from grain_size_times.txt
grain_sizes = []
times = []
number_of_tasks = []
with open('/workspace/grain_size_times.txt', 'r') as file:
    for line in file:
        grain_size, time_taken, task_count = line.split()
        grain_sizes.append(int(grain_size))
        times.append(int(time_taken))
        number_of_tasks.append(int(task_count))

# Plot
plt.figure()
plt.subplot(2, 1, 1)
plt.plot(grain_sizes, number_of_tasks, marker='o', color='orange')
plt.xlabel("Grain size")
plt.ylabel("Number of tasks (log scale)")
plt.yscale("log")
plt.title("Grain size vs Number of Tasks (1200x1200, 2 cores)")
plt.subplot(2, 1, 2)
plt.plot(grain_sizes, times, marker='o')
plt.xlabel("Grain size")
plt.ylabel("Time taken (microseconds)")
plt.title("Grain size vs Execution Time (1200x1200, 2 cores)")
plt.show()