const dataDoMesta = require('./doMesta.json');
const dataZMesta = require('./zMesta.json');

const currentTime = new Date();
const currentHour = currentTime.getHours();
const currentMinute = currentTime.getMinutes();

const numberOfDepartures = 5;

/**
 * Filter bus data by current time and get next N departures
 * @param {Object} busData - The JSON data (zMesta or doMesta)
 * @param {number} currentHour - Current hour (0-23)
 * @param {number} currentMinute - Current minute (0-59)
 * @param {number} n - Number of next departures to return
 * @returns {Array} Array of next departures with bus number and time
 */
function getNextDepartures(busData, currentHour, currentMinute, n) {
  const allDepartures = [];
  const currentTimeInMinutes = currentHour * 60 + currentMinute;

  // Loop through each bus number
  Object.keys(busData).forEach(busNumber => {
    const schedule = busData[busNumber];

    // Loop through each hour
    Object.keys(schedule).forEach(hour => {
      const hourNum = parseInt(hour);
      const minutes = schedule[hour];

      // Loop through each minute in that hour
      minutes.forEach(minute => {
        const minuteNum = parseInt(minute);
        const departureTimeInMinutes = hourNum * 60 + minuteNum;

        // Only include future departures
        if (departureTimeInMinutes > currentTimeInMinutes) {
          allDepartures.push({
            busNumber: busNumber,
            hour: hourNum,
            minute: minuteNum,
            time: `${String(hourNum).padStart(2, '0')}:${String(minuteNum).padStart(2, '0')}`,
            totalMinutes: departureTimeInMinutes
          });
        }
      });
    });
  });

  // Sort by time
  allDepartures.sort((a, b) => a.totalMinutes - b.totalMinutes);

  // Return only the first N with custom format
  const odjezdy = allDepartures.slice(0, n).map(dep => {
    const minutesUntil = dep.totalMinutes - currentTimeInMinutes;
    const timeText = minutesUntil === 1 ? 'minuta' : minutesUntil < 5 ? 'minuty' : 'minut';
    return `${dep.busNumber}   ${dep.time}   za ${minutesUntil} ${timeText}`;
  });

  return {
    odjezdy: odjezdy
  };
}

// Example usage:
const nextBuses = getNextDepartures(dataZMesta, currentHour, currentMinute, numberOfDepartures);
console.log(nextBuses);

