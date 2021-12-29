import './App.css';
import React from 'react';
import SensorData from './components/SensorData';
import ThermostatModes from './components/ThermostatModes';

function App() {
  return (
    <div className='layout'>
      <div className='main'>
        <SensorData tempData={[null, null]}/>
        <ThermostatModes />
      </div>
    </div>
  );
}

export default App;


