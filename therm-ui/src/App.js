import './App.css';
import React from 'react';
import SensorData from './components/SensorData';
import ThermostatModes from './components/ThermostatModes';
import Schedule from './components/Schedule';
import LoadingScreen from './components/Loading';

class App extends React.Component {
  state = { data: null, isLoading: true, error: null };

  async componentDidMount() {
    try {
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode');
      const data = await response.json();
      this.setState({ data: data, isLoading: false });
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
    }
  }

  renderCardsData() {
    
    return (
      <>
      <SensorData tempData={this.state.data.temperatures}/>
      <ThermostatModes radioValue={this.state.data.mode}/>
      <Schedule 
        weekdayScheduleCool={this.state.data.weekdayScheduleCool}
        weekendScheduleCool={this.state.data.weekendScheduleCool}
        weekdayScheduleHeat={this.state.data.weekdayScheduleHeat}
        weekendScheduleHeat={this.state.data.weekendScheduleHeat}
      />
      </>
    )
  }

  renderCardsLoading() {
    return (
      <LoadingScreen />
    )
  }

  renderCardsError() {
    return (
      <>
      <SensorData tempData={[null, null]}/>
      <ThermostatModes radioValue={null}/>
      <Schedule />
      </>
    )
  }


  render() {
    return (
      <div className='layout'>
        <div className='main'>
          {this.state.data ?
          this.renderCardsData() :
          (this.state.error ?
          this.renderCardsError() :
          this.renderCardsLoading())}
        </div>
      </div>
    );
  }
}

export default App;


