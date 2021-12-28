import './App.css';
import React, { useState } from 'react';
import Button from 'react-bootstrap/Button';
import Card from 'react-bootstrap/Card';
import { ButtonGroup, ToggleButton } from 'react-bootstrap';

class ThermostatModes extends React.Component {
  state = { radioValue: null, isLoading: true, error: null };

  async componentDidMount() {
    try {
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode');
      const data = await response.json();
      this.setState({ radioValue: data.mode.toString(), isLoading: false });

    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
    }
  }

  render() {
    const radios = [
      { name: 'Heat', value: '1' },
      { name: 'Cool', value: '2' },
      { name: 'Fan', value: '3' },
      { name: 'Off', value: '0' },
    ];
  
    const variants = {
      0: 'outline-danger',
      1: 'outline-info',
      2: 'outline-warning',
      3: 'outline-dark',
    }

    return (
      <>
      <ButtonGroup>
        {radios.map((radio, idx) => (
          <ToggleButton
            key={idx}
            id={`radio-${idx}`}
            type="radio"
            variant={variants[idx]}
            name="radio"
            value={radio.value}
            checked={this.state.radioValue === radio.value}
            onChange={(e) => {
              this.setState({radioValue: e.currentTarget.value})
              try {
                const requestOptions = {
                  method: 'POST',
                  headers: { 'Content-Type': 'application/json' },
                  body: JSON.stringify({ mode: e.currentTarget.value })
                };
                const response = fetch('http://192.168.1.32:5000/thermostat/mode', requestOptions);
              } catch (error) {
                this.setState({ error: error.message, isLoading: false });
              }
            }}
          >
            {radio.name}
          </ToggleButton>
        ))}
      </ButtonGroup>
    </>
    );
  }
}

class App extends React.Component {
  state = { temp: null, isLoading: true, error: null };


  renderTemp = () => {
    const { temp, isLoading, error } = this.state;
    if (error) {
      return <div>{error}</div>;
    }

    if (isLoading) {
      return <span>Loading...</span>;
    }

    return (
      <span>{temp}</span>
    );
  };

  render() {
    return (
      <div className='layout'>
        <div className='main'>
          <Card>
          <Card.Body>
            <Card.Title>Sensor Data:</Card.Title>
            <Card.Text>
              <p>Sensor 1 temperature: null</p>
              <p>Sensor 2 temperature: null</p>
              <p>Average temperature: null</p>
            </Card.Text>
            <Button variant="primary" disabled>Refresh</Button>
          </Card.Body>
        </Card>
        <Card>
          <Card.Body>
            <Card.Title>Thermostat Mode:</Card.Title>
            <Card.Text style={{textAlign: "center"}}>
            <ThermostatModes />
            </Card.Text>
          </Card.Body>
        </Card>
        </div>
      </div>
    );
  }
}

export default App;


