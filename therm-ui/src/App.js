import './App.css';
import React from 'react';
import SensorData from './components/SensorData';
import ThermostatModes from './components/ThermostatModes';
import ScheduleCard from './components/ScheduleCard';
import { Spinner } from 'react-bootstrap';
import "react-notifications/lib/notifications.css";
import { NotificationContainer } from 'react-notifications';

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
      <ScheduleCard 
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
      // <LoadingScreen />
      <div style={{textAlign: "center", marginTop: "20px"}}>
        <Spinner animation="border"/>
        <h1 style={{display: "inline"}}>&nbsp;&nbsp;Fetching Data from Thermostat...</h1>
      </div>
      
    )
  }

  renderCardsError() {
    return (
      <>
      <h1>An error connecting to the thermostat has occurred</h1>
      </>
    )
  }


  render() {
    return (
      <div className='layout'>
        <NotificationContainer />
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


