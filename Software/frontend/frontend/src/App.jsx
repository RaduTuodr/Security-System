import React, { useState, useEffect } from "react";

const App = () => {
  const [status, setStatus] = useState({
    machineEnabled: false,
    beamBroken: false,
    lastKeys: ["", "", "", ""],
  });

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch("http://localhost:8000/status");
        const data = await response.json();
        setStatus({
          machineEnabled: data.machineEnabled,
          beamBroken: data.beamBroken,
          lastKeys: data.lastKeys,
        });
      } catch (error) {
        console.error("Error fetching status:", error);
      }
    };

    const interval = setInterval(fetchData, 500);
    return () => clearInterval(interval);
  }, []);

  return (
    <div style={styles.app}>
      <div style={styles.card}>
        <h1 style={styles.title}>Laser Tripwire System</h1>

        <div style={styles.statusSection}>
          <div style={status.machineEnabled ? styles.enabledBadge : styles.disabledBadge}>
            {status.machineEnabled ? "Machine Enabled" : "Machine Disabled"}
          </div>

          <div style={status.beamBroken ? styles.brokenBadge : styles.intactBadge}>
            {status.beamBroken ? "Laser Beam Broken" : "Laser Beam Intact"}
          </div>
        </div>

        <div style={styles.pinSection}>
          {status.lastKeys.map((key, index) => (
            <div key={index} style={styles.pinDigit}>
              {key || "•"} {/* Show the digit or a placeholder dot */}
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

const styles = {
  app: {
    display: "flex",
    justifyContent: "center",
    alignItems: "center",
    height: "100vh",
    width: "100vw",
    margin: 0,
    background: "linear-gradient(135deg, #1f1f1f, #292929, #0f0f0f)",
    fontFamily: "Arial, sans-serif",
  },
  card: {
    background: "rgba(255, 255, 255, 0.1)",
    backdropFilter: "blur(10px)",
    borderRadius: "20px",
    padding: "40px",
    width: "90%",
    maxWidth: "500px",
    textAlign: "center",
    boxShadow: "0px 8px 16px rgba(0, 0, 0, 0.3)",
  },
  title: {
    fontSize: "2.5rem",
    fontWeight: "bold",
    color: "#fff",
    marginBottom: "20px",
  },
  statusSection: {
    display: "flex",
    justifyContent: "space-around",
    marginBottom: "30px",
  },
  enabledBadge: {
    padding: "10px 20px",
    borderRadius: "50px",
    background: "#28a745",
    color: "#fff",
    fontWeight: "bold",
    fontSize: "1rem",
  },
  disabledBadge: {
    padding: "10px 20px",
    borderRadius: "50px",
    background: "#dc3545",
    color: "#fff",
    fontWeight: "bold",
    fontSize: "1rem",
  },
  intactBadge: {
    padding: "10px 20px",
    borderRadius: "50px",
    background: "#007bff",
    color: "#fff",
    fontWeight: "bold",
    fontSize: "1rem",
  },
  brokenBadge: {
    padding: "10px 20px",
    borderRadius: "50px",
    background: "#ffc107",
    color: "#000",
    fontWeight: "bold",
    fontSize: "1rem",
  },
  pinSection: {
    display: "flex",
    justifyContent: "center",
    gap: "15px",
  },
  pinDigit: {
    width: "40px",
    height: "40px",
    fontSize: "1.5rem",
    fontWeight: "bold",
    color: "#fff",
    background: "rgba(255, 255, 255, 0.3)",
    border: "2px solid rgba(255, 255, 255, 0.5)",
    borderRadius: "10px",
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
  },
};

export default App;
