import pandas as pd
import matplotlib.pyplot as plt

# Read CSV file
data = pd.read_csv("human_results.csv")

# Plot graph
plt.plot(data["n"], data["time_seconds"], marker='o')

plt.xlabel("Number of Variables (n)")
plt.ylabel("Execution Time (seconds)")
plt.title("Human Version: Time vs n")

plt.grid(True)

# Save graph
plt.savefig("human_time_vs_n.png", dpi=300)

plt.show()
