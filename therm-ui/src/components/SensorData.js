import Button from 'react-bootstrap/Button';
import Card from 'react-bootstrap/Card';
import PropTypes from 'prop-types';
import lodash from 'lodash';

function SensorData(props) {
  SensorData.propTypes = {
    tempData: PropTypes.array.isRequired
  }

  return (
    <Card>
      <Card.Body>
        <Card.Title>Sensor Data:</Card.Title>
          <Card.Text>
            {props.tempData.map((value, index) => {
              return <p>Sensor {index} temperature: {value}</p>
            })}
            {/* TODO: Make the average temp null if there is no data */}
            <p>Average temperature: {lodash.sum(props.tempData) / props.tempData.length}</p>
          </Card.Text>
        <Button variant="primary" disabled>Refresh</Button>
      </Card.Body>
    </Card>
  );
}

export default SensorData;