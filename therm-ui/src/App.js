import logo from './logo.svg';
import './App.css';
import React from 'react';
import Button from 'react-bootstrap/Button';
import Card from 'react-bootstrap/Card';

class App extends React.Component {
  state = { temp: null, isLoading: true, error: null };

  async componentDidMount() {
    try {
      const response = await fetch('http://192.168.1.32:5000/tempurature');
      const data = await response.json();
      this.setState({ temp: data.temp, isLoading: false });

    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
    }
  }

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
            <Button variant="primary">Refresh</Button>
          </Card.Body>
        </Card>
        </div>
      </div>
    );
  }
}

export default App;


