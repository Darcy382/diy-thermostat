import Button from 'react-bootstrap/Button';
import Card from 'react-bootstrap/Card';
import PropTypes from 'prop-types';
import lodash from 'lodash';
import React from 'react';
import NotificationManager from 'react-notifications/lib/NotificationManager';
import { Spinner } from 'react-bootstrap';

class SensorData extends React.Component {

  state = {tempData: this.props.tempData, isLoading: false}

  refreshData = async () => {
    this.setState({isLoading: true});
    try {
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode');
      const data = await response.json();
      this.setState({ 
        tempData: data.temperatures,
        isLoading: false 
      });
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error("Error", "", 1000);
    }
  }

  renderDataState = () => (
    <>
      {this.state.tempData.map((value, index) => {
        return <p key={index}>Sensor {index} temperature: {value}˚F</p>
      })}
      {/* TODO: Make the average temp null if there is no data */}
      <p>Average temperature: {(lodash.sum(this.state.tempData) / this.state.tempData.length).toFixed(2)}˚F</p>
    </>
  );

  renderLoadingState = () => (
    <>
      {this.state.tempData.map((value, index) => {
        return <p key={index}>Sensor {index} temperature: <Spinner animation="border" size="sm" />˚F</p>
      })}
      {/* TODO: Make the average temp null if there is no data */}
      <p>Average temperature: <Spinner animation="border" size="sm" />˚F</p>
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
  tempData: PropTypes.array.isRequired
}

export default SensorData;