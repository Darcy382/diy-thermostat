import Button from 'react-bootstrap/Button';
import Card from 'react-bootstrap/Card';
import PropTypes from 'prop-types';
import lodash from 'lodash';
import React from 'react';
import NotificationManager from 'react-notifications/lib/NotificationManager';
import { Spinner } from 'react-bootstrap';

class SensorData extends React.Component {

  state = {sensorData: this.props.sensorData, isLoading: false}

  refreshData = async () => {
    this.setState({isLoading: true});
    try {
      const response = await fetch('http://192.168.1.25:5000/thermostat/mode');
      const data = await response.json();
      this.setState({ 
        sensorData: data.sensors,
        isLoading: false 
      });
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error("Error", "", 1000);
    }
  }

  renderDataState = () => (
    <>
      {this.state.sensorData.map((sensorObj, index) => {
        return (
        <p key={index}><h6>Sensor {index}: </h6>
          Temperature: {sensorObj.temperature}˚F 
          <br/>Humidity: {sensorObj.humidity}%
          <br/>Heat Index: {sensorObj.heat_idx}˚F 
        </p>
        )
      })}
      {/* TODO: Make the average temp null if there is no data */}
      {/* <p>Average temperature: {(lodash.sum(this.state.sensorData) / this.state.sensorData.length).toFixed(2)}˚F</p> */}
    </>
  );

  renderLoadingState = () => (
    <>
      {this.state.sensorData.map((sensorObj, index) => {
        return (
        <p key={index}><h6>Sensor {index}: </h6>
          Temperature: <Spinner animation="border" size="sm" />˚F 
          <br/>Humidity: <Spinner animation="border" size="sm" />%
          <br/>Heat Index: <Spinner animation="border" size="sm" />˚F 
        </p>
        )
      })}
      {/* <p>Average temperature: {(lodash.sum(this.state.sensorData) / this.state.sensorData.length).toFixed(2)}˚F</p> */}
    </>
  );

  render() {
    return (
      <Card>
        <Card.Body>
          <Card.Title>Sensor Data:</Card.Title>
          {this.state.isLoading ? 
          this.renderLoadingState() :
          this.renderDataState()
          }
          <Button variant="primary" onClick={this.refreshData}>Refresh</Button>
        </Card.Body>
      </Card>
    );
  }
}

SensorData.propTypes = {
  sensorData: PropTypes.array.isRequired
}

export default SensorData;