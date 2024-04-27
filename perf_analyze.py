import matplotlib.pyplot as plt
import numpy as np
import re

import re

# log_data = None

# with open("log.log", "r") as f:
#     log_data = f.read()

# # Regex to identify and extract necessary components from the log
# regex_command = r'--mode (\w+) --bladeNumExponent (\d+)'
# regex_avg_frame_time = r'Avg Frame Time: (\d+\.\d+)us'

data = {
    "tess": {  # Tessellation Shader + Compute Shader
        10: [493.439, 419.883, 419.547, 419.751, 418.54, 419.011, 419.262, 419.801, 419.035, 418.896, 419.865, 418.261, 417.463, 419.728, 420.228, 419.001, 419.293, 418.657, 418.216],
        12: [593.62, 545.785, 546.587, 545.962, 545.495, 547.584, 546.477, 548.688, 547.82, 546.331, 547.88, 548.294, 548.312, 546.776, 548.357, 546.324, 545.869, 546.171, 545.03],
        14: [1052.04, 945.769, 944.866, 941.877, 941.324, 941.439, 938.202, 939.861, 941.007, 941.12, 944.758, 945.195, 947.045, 946.546, 945.245, 942.868, 941.089, 938.081, 937.808],
        16: [2692.12, 2488.08, 2483.77, 2475.08, 2467.13, 2463.22, 2463.57, 2461.22, 2468.59, 2479.38, 2480.07, 2485.86, 2488.25, 2488.89, 2487.4, 2478.3, 2474.68, 2463.13, 2452.1],
        18: [9478.1, 9158.78, 9144.2, 9101.73, 9069.27, 9010.03, 8982.15, 9013.9, 9060.49, 9097.9, 9115.21, 9132.98, 9138.51, 9157.53, 9154.82, 9145.94, 9070.77, 9009.58, 8988.56],
        20: [32348.1, 35217.7, 35101.8, 34907.3, 34901.7, 34812.1, 34812.7, 34830.2, 34958.6, 35058.1, 35224.2, 35319.7, 35338.3, 35266.8, 35169, 35018.4, 34909.3, 34809.4, 34850]
    },
    "mesh": {
        10: [460.911, 420.527, 421.288, 419.851, 423.959, 420.555, 422.172, 421.82, 420.569, 420.389, 421.261, 421.514, 420.662, 421.727, 421.431, 419.877, 421.156, 421.246, 420.12],
        12: [568.61, 521.029, 521.879, 523.155, 529.644, 521.343, 523.639, 523.713, 522.536, 522.792, 522.25, 522.125, 523.124, 523.642, 523.192, 522.573, 520.849, 522.557, 521.468],
        14: [906.307, 835.449, 835.967, 833.854, 831.239, 833.941, 835.759, 835.157, 835.261, 838.992, 837.074, 835.387, 835.57, 836.339, 835.523, 834.541, 834.186, 834.155, 833.687],
        16: [2299.32, 2136.03, 2131.32, 2129.52, 2130.45, 2128.76, 2133.41, 2130.97, 2130.08, 2132.79, 2130.52, 2131.09, 2145.83, 2130.77, 2132.8, 2130.87, 2129.42, 2128.48, 2129.97],
        18: [7854.36, 7491.27, 7487.33, 7487.31, 7490.64, 7507.05, 7503.86, 7486.4, 7490.63, 7498.82, 7526.32, 7490.79, 7481.92, 7481.73, 7489.67, 7486.64, 7491.78, 7486.41, 7482.86],
        20: [27252.2, 28790.9, 28816.7, 28795.9, 28771.3, 28886, 28816.3, 28818.8, 28766, 28798.6, 28845.1, 28799.4, 28792.8, 28816.3, 28790.3, 28816.7, 28801, 28823.5, 28797.7],
    },
    "mesh_compute": {
        10: [441.544, 407.557, 406.912, 406.56, 406.213, 404.732, 405.261, 406.431, 406.673, 407.083, 406.594, 406.228, 407.003, 407.454, 408.502, 411.257, 407.635, 403.794, 405.592],
        12: [503.299, 461.916, 461.573, 461.895, 462.998, 462.547, 461.033, 463.276, 461.98, 462.892, 462.538, 462.253, 463.454, 464.122, 463.192, 461.774, 462.795, 461.573, 467.195],
        14: [666.294, 607.73, 607.303, 606.15, 606.453, 606.215, 605.215, 605.609, 607.108, 606.783, 608.439, 607.75, 606.46, 607.422, 607.353, 607.855, 608.355, 604.33, 605.827],
        16: [1343.76, 1239.97, 1238.99, 1237.76, 1238.13, 1239.78, 1239.46, 1237.99, 1237.73, 1238.52, 1237.23, 1239.26, 1239.21, 1237.28, 1237.73, 1239.46, 1241.15, 1237.38, 1239.86],
        18: [4070.73, 3814.22, 3809.76, 3811.49, 3812.66, 3817.17, 3817.46, 3815.86, 3813.33, 3808.93, 3806.99, 3810.92, 3809.94, 3806.26, 3809.77, 3809.96, 3813.79, 3816.37, 3814.73],
        20: [14103.5, 14040.7, 14028.8, 14034.8, 14052.1, 14057.8, 14068.9, 14045.5, 14061.7, 14068.1, 14054.7, 14045.8, 14040.1, 14030.7, 14034.6, 14058.1, 14044.5, 14049.9, 14061.7],
    }
}


import matplotlib.pyplot as plt
import numpy as np

# Extracting data
exponents = list(data['tess'].keys())
tess_means = [np.mean(data['tess'][exp][1:]) for exp in exponents]
mesh_means = [np.mean(data['mesh'][exp][1:]) for exp in exponents]
mesh_compute_means = [np.mean(data['mesh_compute'][exp][1:]) for exp in exponents]

# Plotting
plt.figure(figsize=(12, 8))
bar_width = 0.25
x_indexes = np.arange(len(exponents))

# # Creating bars and adding data labels
# bars_tess = plt.bar(x_indexes, tess_means, width=bar_width, label='Tessellation Shader + Compute Shader')
# bars_mesh = plt.bar(x_indexes + bar_width, mesh_means, width=bar_width, label='Mesh Shader + Compute Shader')
# bars_mesh_compute = plt.bar(x_indexes + 2 * bar_width, mesh_compute_means, width=bar_width, label='Mesh Shader Standalone')

# # Function to add labels on top of each bar
# def add_labels(bars):
#     for bar in bars:
#         height = bar.get_height()
#         plt.text(bar.get_x() + bar.get_width() / 2, height, f'{height:.2f}', ha='center', va='bottom', fontsize=8)

# # Adding labels to all sets of bars
# add_labels(bars_tess)
# add_labels(bars_mesh)
# add_labels(bars_mesh_compute)

# plt.xlabel('Exponent of Grass Blade Count (2^x)')
# plt.ylabel('Mean Frame Time (ms)')
# plt.title('Comparison of Shader Methods by Grass Blade Count')
# plt.yscale('log')
# plt.xticks(x_indexes + bar_width, [f'2^{exp}' for exp in exponents])
# plt.legend()

# plt.show()

# Creating bars and adding data labels
bars_tess = plt.bar(x_indexes, tess_means, width=bar_width, label='Tessellation Shader + Compute Shader')
bars_mesh = plt.bar(x_indexes + bar_width, mesh_means, width=bar_width, label='Mesh Shader + Compute Shader')
bars_mesh_compute = plt.bar(x_indexes + 2 * bar_width, mesh_compute_means, width=bar_width, label='Mesh Shader Standalone')

# Function to add labels on top of each bar
def add_labels(bars):
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width() / 2, height+1, f'{height:.2f}', ha='center', va='bottom', fontsize=8)

# Adding labels to all sets of bars
add_labels(bars_tess)
add_labels(bars_mesh)
add_labels(bars_mesh_compute)

# Adding lines connecting the mid points of the bars
mid_points_tess = [bar.get_height() for bar in bars_tess]
mid_points_mesh = [bar.get_height() for bar in bars_mesh]
mid_points_mesh_compute = [bar.get_height() for bar in bars_mesh_compute]

plt.plot(x_indexes + 0 * bar_width, mid_points_tess, color='blue', marker='o')
plt.plot(x_indexes + 1 * bar_width, mid_points_mesh, color='orange', marker='o')
plt.plot(x_indexes + 2 * bar_width, mid_points_mesh_compute, color='green', marker='o')

plt.xlabel('Exponent of Grass Blade Count (2^x)')
plt.ylabel('Mean Frame Time (us)')
plt.title('Grass Simulation & Rendering Performance Comparison by Grass Blade Count')
plt.yscale('log')
plt.xticks(x_indexes + bar_width, [f'2^{exp}' for exp in exponents])
plt.legend()

plt.show()

print("Tessellation Shader + Compute Shader")
for i in range(len(exponents)):
    print(f"2^{exponents[i]}: {tess_means[i]}")

print("Mesh Shader + Compute Shader")
for i in range(len(exponents)):
    print(f"2^{exponents[i]}: {mesh_means[i]}")

print("Mesh Shader Standalone")
for i in range(len(exponents)):
    print(f"2^{exponents[i]}: {mesh_compute_means[i]}")


# # Data
# light_source_counts = [1, 2, 4, 8, 16, 32, 64, 128, 256, 511]
# average_frame_times = [385.996, 403.923, 446.864, 550.047, 839.341, 1393.075, 2564.878, 5797.862, 12944.406, 26972.488]

# # Plotting
# plt.figure(figsize=(10, 6))
# plt.plot(light_source_counts, average_frame_times, marker='o', linestyle='-', color='b')
# plt.title('Average Frame Time vs. Light Source Count')
# plt.xlabel('Light Source Count')
# plt.ylabel('Average Frame Time (us)')
# plt.grid(True)
# plt.xscale('log')
# plt.yscale('log')
# plt.xticks(light_source_counts, labels=light_source_counts)
# plt.tight_layout()
# plt.show()

# for i in range(1, len(average_frame_times)):
#     print(average_frame_times[i] / light_source_counts[i])


# Use these data to generate a comparison bar graph of three methods:

# - Tessellation Shader + Compute Shader
# - Mesh Shader + Compute Shader
# - Mesh Shader

#  under different workload: grass blade count from 2^10 to 2^20

# It's better to use a logaxis on frametime for Y axis and the exponent as the X axis. You can group the three methods for the same grass blade count.


# Can you also write a parser to every data to exclude the first sample, then average the frame time of the 20 seconds.