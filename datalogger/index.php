<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta http-equiv="refresh" content="300">
		<title>ECE 331 Datalogger</title>
		<script src="https://cdn.jsdelivr.net/npm/moment@2.29.3/moment.min.js"></script>
		<script src="https://cdn.jsdelivr.net/npm/chart.js@3.7.1/dist/chart.min.js"></script>
		<script src="https://cdn.jsdelivr.net/npm/chartjs-adapter-moment@1.0.0/dist/chartjs-adapter-moment.min.js"></script>
	</head>
	<h1><center>ECE 331 Datalogger</h1>
	<body>
		<div><center>Current date and time: <?php echo gmdate("D, M d Y H:i e"); ?></div>
		<hr>
		<div class="chart-container" style="position: relative; left: 50%; transform: translateX(-50%); height:60vh; width:80vw">
			<canvas id="myChart"></canvas>
		</div> <!-- class="chart-container" -->
		<script>
			plotData();

			async function plotData()
			{
				// Wait until the data is ready before plotting it
				const data = await getData();
				// Set the wanted font color and size
				Chart.defaults.font.size = 18;
				Chart.defaults.color = 'black';
				// Plot the chart onto the canvas called myChart
				const ctx = document.getElementById('myChart').getContext('2d');
				const myChart = new Chart(ctx, {
					type: 'line',
					data: {
						labels: data.time,
						datasets: [{
							label: 'CPU Temperature',
							data: data.core,
							backgroundColor: 'rgba(66, 135, 245, 1)',
							borderColor: 'rgba(7, 58, 59, 1)',
							borderWidth: 1
						},{
							label: 'Room Temperature',
							data: data.room,
							backgroundColor: 'rgba(250, 108, 7, 1)',
							borderColor: 'rgba(158, 90, 0, 1)',
							borderWidth: 1
						}]
					},
					options: {
						scales: {
							y: {
								beginAtZero: false,
								title: {
									display: true,
									text: 'Temperature (\u00B0C)'
								},
								grid: {
									color: 'black'
								}
							},
							x: {
								type: 'time',
								time: {
									unit: 'minute',
									displayFormats: {
										minute: 'YYYY-MM-DD HH:mm'
									}
								},
								beginAtZero: false,
								title: {
									display: true,
									text: 'UTC Date and Time (YYYY-MM-DD HH:mm)'
								},
								grid: {
									color: 'black'
								}
							}
						},
						responsive: true,
						maintainAspectRatio: false,
						plugins: {
							title: {
								display: true,
								text: 'RPi 4 Temperature Logger'
							}
						}
					}
				});
			}
			// This function gets the data from the SQL database, then returns it
			async function getData() {
				// Get the data from the sql database
				const response = await fetch('fetch.php');
				const data = await response.json();

				// Extract the data into separate arrays
				const time = [];
				const room = [];
				const core = [];
				const len = data.length;
				for (let i = 0; i < len; i++) {
					time.push(data[i].time);
					room.push(data[i].room);
					core.push(data[i].core);
				}
				return {time, room, core};
			}
		</script>
	</body>
</html>
