async function fetchSensorData() {
  try {
    const response = await fetch('/api/sensor-readings');
    const data = await response.json();
    const container = document.getElementById('sensors-container');
    const summary = document.getElementById('sensor-summary');
    container.innerHTML = ''; // Xoá spinner

    if (!data.success || data.readings.length === 0) {
      container.innerHTML = '<p class="text-center">No sensor data found.</p>';
      return;
    }

    // Dùng Map để chỉ giữ lại bản ghi mới nhất theo sensor_id
    const latestBySensor = new Map();
    data.readings.forEach(reading => {
      const id = reading.sensor_id;
      if (!latestBySensor.has(id)) {
        latestBySensor.set(id, reading);
      }
    });

    // Sắp xếp theo sensor_id để cố định thứ tự hiển thị
    const sortedSensors = Array.from(latestBySensor.values())
      .sort((a, b) => a.sensor_id - b.sensor_id);

    sortedSensors.forEach(reading => {
      const card = document.createElement('div');
      card.className = 'col-md-4 mb-3';

      card.innerHTML = `
        <div class="card border-info">
          <div class="card-header text-white bg-info">
            ${reading.sensor_type || 'Sensor'} - ID ${reading.sensor_id}
        </div>
        <div class="card-body">
            <p><strong>Value:</strong> ${reading.value1 ?? 'N/A'} ${reading.unit1 || ''}</p>
            <p><strong>Time:</strong> ${reading.timestamp}</p>
          </div>
        </div>
      `;
      container.appendChild(card);
    });


  } catch (error) {
    console.error('Lỗi khi tải dữ liệu cảm biến:', error);
    document.getElementById('sensors-container').innerHTML =
      '<p class="text-danger">Failed to load sensor data.</p>';
  }
  // Giả sử readings đã có sensor_id và sensor_type
  function populateSensorDropdown(readings) {
  const select = document.getElementById("sensorSelect");
  const ids = new Set();
  readings.forEach(r => {
    if (!ids.has(r.sensor_id)) {
      ids.add(r.sensor_id);
      const option = document.createElement("option");
      option.value = r.sensor_id;
      option.textContent = `${r.sensor_type} (ID ${r.sensor_id})`;
      select.appendChild(option);
    }
  });
}

}

document.addEventListener("DOMContentLoaded", () => {
  fetchSensorData(); // gọi lần đầu khi trang tải
  setInterval(fetchSensorData, 5000); // cập nhật mỗi 5 giây
});
