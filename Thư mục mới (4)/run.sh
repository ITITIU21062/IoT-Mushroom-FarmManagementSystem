#!/bin/bash

# === Thư mục làm việc ===
cd "$(dirname "$0")"

# === Kiểm tra và dừng tiến trình đang chiếm cổng 5000 ===
PORT=5000
PID=$(lsof -ti :$PORT)

if [ -n "$PID" ]; then
  echo "🛑 Dừng tiến trình đang chạy trên cổng $PORT (PID: $PID)..."
  kill -9 $PID
else
  echo "✅ Cổng $PORT đang trống, tiếp tục chạy ứng dụng."
fi

# === Kích hoạt virtual environment ===
source venv/bin/activate

# === Chạy Flask App ===
echo "🚀 Đang khởi động Flask app trên cổng $PORT..."
python3 app.py
