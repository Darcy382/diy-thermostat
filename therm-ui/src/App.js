import './App.css';
import React from 'react';
import SensorData from './components/SensorData';
import ThermostatModes from './components/ThermostatModes';
import Schedule from './components/Schedule';

class App extends React.Component {

  render() {
    return (
      <div className='layout'>
        <div className='main'>
          <SensorData tempData={[null, null]}/>
          <ThermostatModes />
          <Schedule />
        </div>
      </div>
    );
  }
}

export default App;


