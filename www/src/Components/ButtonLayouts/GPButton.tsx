import React from 'react';
import GPShape from './GPShape';
import {
	BUTTONS,
    BUTTON_MASKS,
} from '../../Data/Buttons';

class GPButton extends GPShape {
    constructor(props) {
        super(props);
        this.clickHandler = this.clickHandler.bind(this);
    }

    getDetails() {
        let details = {
            elementType: 'button',
            buttonType: '',
            buttonValue: '',
            buttonPin: -1,
        };

        if (this.props.elementDetails) {
            switch (this.props.elementDetails.elementType) {
                case 2:
                    details.buttonType = 'button';
                    details.buttonValue = BUTTON_MASKS.find((o,i) => o.value == this.props.value).label;
                    details.buttonPin = -1;
                    break;
                case 3:
                    details.buttonType = 'direction';
                    details.buttonValue = BUTTON_MASKS.find((o,i) => o.value == (this.props.value << 16)).label;
                    details.buttonPin = -1;
                    break;
                case 4:
                    details.buttonType = 'pin';
                    details.buttonPin = this.props.value;
                    break;
                default:
                    break;
            }
        }

        return details;
    }
}

export default GPButton;