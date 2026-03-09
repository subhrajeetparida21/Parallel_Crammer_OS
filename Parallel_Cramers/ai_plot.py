import pandas as pd
import matplotlib.pyplot as plt

# Read CSV file
data = pd.read_csv("ai_results.csv")

# Plot graph
plt.plot(data["n"], data["time_seconds"], marker='o')

plt.xlabel("Number of Variables (n)")
plt.ylabel("Execution Time (seconds)")
plt.title("AI Version: Time vs n")

plt.grid(True)

# Save graph
plt.savefig("ai_time_vs_n.png", dpi=300)

plt.show()
