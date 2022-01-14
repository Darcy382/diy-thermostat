import React from 'react';
import { ButtonGroup, Card, Col, Row, ToggleButton } from 'react-bootstrap';
import NotificationManager from 'react-notifications/lib/NotificationManager';
import { DEFAULT_NOTIFICATION_TIME } from '../constants';
import { API_HOSTNAME } from '../userSettings';

const thermModes = [
  { name: 'Heat', value: 1, buttonStyle: 'outline-danger'},
  { name: 'Cool', value: 2, buttonStyle: 'outline-info'},
  { name: 'Off', value: 0, buttonStyle: 'outline-dark'},
];

const fanSettings = [
  { name: 'Auto', value: 0, buttonStyle: 'outline-success'},
  { name: 'Always on', value: 1, buttonStyle: 'outline-warning'},
];

class ThermostatModes extends React.Component {
  state = { thermMode: this.props.thermMode, fanSetting: this.props.fanSetting, isLoading: true, error: null };

  // TODO: Figure out why this works and it is not in an arrow function
  async setThermostatMode(mode) {
    this.setState({thermMode: mode})
    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ mode })
      };
      const response = await fetch(`http://${API_HOSTNAME}/thermostat/mode`, requestOptions);
      const data = await response.json();
      this.setState({ thermMode: data.mode, isLoading: false });
      NotificationManager.success("Thermostat mode changed", "Success", DEFAULT_NOTIFICATION_TIME);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error(error.message, "Error", DEFAULT_NOTIFICATION_TIME);

    }
  }

  async setFanSetting(fanSetting) {
    this.setState({fanSetting: fanSetting})
    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ fanSetting })
      };
      const response = await fetch(`http://${API_HOSTNAME}/thermostat/mode`, requestOptions);
      const data = await response.json();
      this.setState({ fanSetting: data.fanSetting, isLoading: false });
      NotificationManager.success("Fan setting updated", "Success", DEFAULT_NOTIFICATION_TIME);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error(error.message, "Error", DEFAULT_NOTIFICATION_TIME);
    }
  }
  
  render() {
    return (
      <Card>
      <Card.Body>
        <Card.Title>Controls:</Card.Title>
        <div style={{textAlign:"center"}}>
        <Row>
          <Col>
          <h5 style={{display: "inline"}}>Thermostat: &nbsp;&nbsp;</h5> 
          <ButtonGroup name="group1" className='controls'>
            {thermModes.map((radio, idx) => (
              <ToggleButton
                key={`therm-${idx}`}
                id={`therm-${idx}`}
                type="radio"
                variant={radio.buttonStyle}
                name="thermRadio"
                value={radio.value}
                checked={this.state.thermMode === radio.value}
                onChange={(e) => {
                  this.setThermostatMode(parseInt(e.currentTarget.value))
                }}
              >
                {radio.name}
              </ToggleButton>
            ))}
          </ButtonGroup>
          </Col>
          
          <Col>
          <h5 style={{display: "inline"}}>Fan: &nbsp;&nbsp;</h5> 
          <ButtonGroup name="group2" className='controls'>
            {fanSettings.map((radio, idx) => (
              <ToggleButton
                key={`fan-${idx}`}
                id={`fan-${idx}`}
                type="radio"
                variant={radio.buttonStyle}
                name="fanRadio"
                value={radio.value}
                checked={this.state.fanSetting === radio.value}
                onChange={(e) => {
                  this.setFanSetting(parseInt(e.currentTarget.value))
                }}
              >
                {radio.name}
              </ToggleButton>
            ))}
          </ButtonGroup>
          </Col>
        </Row>
        </div>
        </Card.Body>
      </Card>
    );
  }
}

export default ThermostatModes;