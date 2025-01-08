import { useEffect, useState } from "react";

function App() {
  const [status, setStatus] = useState({
    machineEnabled: false,
    beamBroken: false,
    lastKeyPressed: null,
  });

  useEffect(() => {
    // poll the /status endpoint every 1s
    const intervalId = setInterval(() => {
      fetch("http://localhost:8000/status")
        .then((res) => res.json())
        .then((data) => {
          setStatus(data);
        })
        .catch((err) => console.error(err));
    }, 1000);

    // Cleanup when the component unmounts
    return () => clearInterval(intervalId);
  }, []);

  return (
    <div style={{ padding: "1rem" }}>
      <h1>Arduino Laser Tripwire</h1>
      <p>
        <strong>Machine Enabled:</strong>{" "}
        {status.machineEnabled ? "Yes" : "No"}
      </p>
      <p>
        <strong>Beam Broken:</strong> {status.beamBroken ? "Yes" : "No"}
      </p>
      <p>
        <strong>Last Key Pressed:</strong> {status.lastKeyPressed ?? "None"}
      </p>
    </div>
  );
}

export default App;
