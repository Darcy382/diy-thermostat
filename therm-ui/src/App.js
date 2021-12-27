import logo from './logo.svg';
import './App.css';
import React from 'react';

class App extends React.Component {
  state = { temp: null, isLoading: true, error: null };

  async componentDidMount() {
    try {
      const response = await fetch('http://192.168.1.32:5000/temp');
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
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <p>
            Current temperature is: {this.renderTemp()}.
          </p>
          <a
            className="App-link"
            href="https://reactjs.org"
            target="_blank"
            rel="noopener noreferrer"
          >
            Learn React
          </a>
        </header>
      </div>
    );
  }
}

export default App;


