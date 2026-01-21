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
# Plot time vs grain size and tasks vs grain size in horizontal subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

ax1.plot(grain_sizes, times, marker='o')
ax1.set_xlabel("Grain Size")
ax1.set_ylabel("Time Taken (microseconds)")
ax1.set_title("Time vs Grain Size (1200x1200 image, 2 cores) (log scale)")
ax1.set_yscale("log")
ax1.set_xscale("log")
ax1.grid(True)

ax2.plot(grain_sizes, number_of_tasks, marker='o')
ax2.set_xlabel("Grain Size")
ax2.set_ylabel("Number of Tasks")
ax2.set_yscale("log")
ax2.set_xscale("log")
ax2.set_title("Tasks vs Grain Size (1200x1200 image, 2 cores) (log scale)")
ax2.grid(True)

plt.tight_layout()
plt.show()