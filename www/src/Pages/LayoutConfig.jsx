import React, { useContext, useEffect, useState } from 'react';
import { AppContext } from '../Contexts/AppContext';
import { Button, Form, FormCheck, FormSelect, Table } from 'react-bootstrap';
import { Formik, useFormikContext, getIn } from 'formik';
import * as yup from 'yup';
import { useTranslation } from 'react-i18next';
import ContextualHelpOverlay from '../Components/ContextualHelpOverlay';

import Section from '../Components/Section';
import WebApi from '../Services/WebApi';
import boards from '../Data/Boards.json';
import {
    BUTTONS,
    MAIN_BUTTONS,
    AUX_BUTTONS,
    KEYBOARD_LAYOUT,
    STICK_LAYOUT,
    STICKLESS_LAYOUT,
} from '../Data/Buttons';
import LEDColors from '../Data/LEDColors';

import './CustomThemePage.scss';

let layoutFieldsSchema = {};

const schema = yup.object().shape({
    ...layoutFieldsSchema
});

let buttonLayouts = {ledLayout:{},displayLayouts:{buttonLayoutId:0,buttonLayout:{},buttonLayoutRightId:0,buttonLayoutRight:{}}};
let buttonLayoutDefinitions = {buttonLayout:{},buttonLayoutRight:{}};

const LEDButton = ({
    id,
    name,
    buttonType,
    buttonColor,
    buttonPressedColor,
    className,
    labelUnder,
    elementProperties,
    onClick,
    ...props
}) => {
    const [pressed, setPressed] = useState(false);

    const handlePressedShow = (e) => {
        // Show pressed state on right-click
        if (e.button === 2) setPressed(true);
    };

    const handlePressedHide = (e) => {
        // Revert to normal state
        setPressed(false);
    };

    buttonColor = '#000000';

    if (elementProperties) {
        //console.dir(elementProperties);

        let leftValue = 0;
        let topValue = 0;
        let widthValue = 0;
        let heightValue = 0;
        let radiusSize = 0;
        let rotationValue = 0;

        if (elementProperties.parameters.shape == 0) {
            // ellipse
            leftValue = (elementProperties.parameters.x1);
            topValue = (elementProperties.parameters.y1);
            widthValue = (elementProperties.parameters.x2);
            heightValue = (elementProperties.parameters.x2);
            radiusSize = 50;
        } else if (elementProperties.parameters.shape == 1) {
            // square
            leftValue = (elementProperties.parameters.x1);
            topValue = ((elementProperties.parameters.y1));
            widthValue = ((elementProperties.parameters.x2-elementProperties.parameters.x1));
            heightValue = ((elementProperties.parameters.y2-elementProperties.parameters.y1));
            radiusSize = 10;
        } else if (elementProperties.parameters.shape == 2) {
            // diamond
            leftValue = (elementProperties.parameters.x1);
            topValue = ((elementProperties.parameters.y1));
            widthValue = ((elementProperties.parameters.x2*2));
            heightValue = ((elementProperties.parameters.x2*2));
            radiusSize = 10;
            rotationValue = 45;
        } else if (elementProperties.parameters.shape == 3) {
            // polygon
            leftValue = (elementProperties.parameters.x1);
            topValue = (elementProperties.parameters.y1);
            widthValue = (elementProperties.parameters.x2);
            heightValue = (elementProperties.parameters.x2);
            radiusSize = 50;
        } else if (elementProperties.parameters.shape == 4) {
            // arc
        }

        //if ((elementProperties.elementType == 2)||(elementProperties.elementType == 3)||(elementProperties.elementType == 4)||(elementProperties.elementType == 8)) {
        //}
        
        return (
            <div
                className={`led-button ${className}`}
                style={{
                    backgroundColor: pressed ? buttonPressedColor : buttonColor, 
                    left: leftValue+'mm', 
                    top: topValue+'mm',
                    width: widthValue+'mm',
                    height: heightValue+'mm',
                    borderRadius: radiusSize != 0 ? `${radiusSize}%` : '',
                    transform: rotationValue != 0 ? `rotate(-${rotationValue}deg)` : null,
                }}
                onClick={onClick}
                onMouseDown={(e) => handlePressedShow(e)}
                onMouseUp={(e) => handlePressedHide(e)}
                onMouseLeave={(e) => handlePressedHide(e)}
                onContextMenu={(e) => e.preventDefault()}
            >
                <span className={`button-label ${labelUnder ? 'under' : ''}`}>
                    {name}
                </span>
            </div>
        );
    } else {
        return (
            <div
                className={`led-button ${className}`}
                style={{ backgroundColor: pressed ? buttonPressedColor : buttonColor }}
                onClick={onClick}
                onMouseDown={(e) => handlePressedShow(e)}
                onMouseUp={(e) => handlePressedHide(e)}
                onMouseLeave={(e) => handlePressedHide(e)}
                onContextMenu={(e) => e.preventDefault()}
            >
                <span className={`button-label ${labelUnder ? 'under' : ''}`}>
                    {name}
                </span>
            </div>
        );
    }
};

const FormContext = () => {
    const { values, setValues } = useFormikContext();
    const { setLoading } = useContext(AppContext);

    useEffect(() => {
        async function fetchData() {
            await WebApi.getGamepadOptions(setLoading);
            buttonLayouts = await WebApi.getButtonLayouts(setLoading);
            buttonLayoutDefinitions = await WebApi.getButtonLayoutDefs(setLoading);
            //console.dir(buttonLayouts);
            //console.dir(buttonLayoutDefinitions);

            //setValues(peripheralOptions);
        }
        fetchData();
    }, [setValues]);

    useEffect(() => {
    }, [values, setValues]);

    return null;
};

export default function LayoutConfigPage() {
    const { buttonLabels, setButtonLabels, usedPins } = useContext(AppContext);
    const [saveMessage, setSaveMessage] = useState('');
    const [selectedButton, setSelectedButton] = useState('');
    const { buttonLabelType } = buttonLabels;

    const onSuccess = async (values) => {
        const cleanValues = schema.cast(values);
        console.dir(cleanValues);

        //const success = await WebApi.setPeripheralOptions(cleanValues);

        setSaveMessage(
            success
                ? t('Common:saved-success-message')
                : t('Common:saved-error-message'),
        );
    };

	const toggleSelectedButton = (e, buttonName) => {
		e.stopPropagation();
        console.dir(selectedButton);
		if (selectedButton === buttonName) {
			//setPickerVisible(false);
		} else {
			//setLedOverlayTarget(e.target);
			//setSelectedButton(buttonName);
			//setSelectedColor(
			//	buttonName === 'ALL' ? '#000000' : customTheme[buttonName].normal,
			//);
			//setPickerType({ type: 'normal', button: buttonName });
			//setPickerVisible(true);
		}
	};

    const { t } = useTranslation('');

    return (
        <Formik onSubmit={onSuccess} validationSchema={schema} initialValues={{}}>
            {({ errors, handleSubmit, setFieldValue, values }) =>
                console.log('errors', errors) || (
                    <div>
                        <Form noValidate onSubmit={handleSubmit}>
                            <Section title={t('LayoutConfig:header-text')}>
                                <div className="row">
                                    <div className="col-6">
                                        <FormSelect
                                            key={`layoutConfigLeft`}
                                            id={`layoutConfigLeft`}
                                            name={`layoutConfigLeft`}
                                            className="form-select-sm sm-1"
                                            error={{}}
                                            value={buttonLayouts.displayLayouts.buttonLayoutId}
                                            onChange={(e) => {}}
                                        >
                                            {Object.keys(buttonLayoutDefinitions.buttonLayout).map((o, i) => (
                                                <option
                                                    key={o}
                                                    value={buttonLayoutDefinitions.buttonLayout[o]}
                                                >
                                                    {t(`LayoutConfig:layouts.left.${o}`)}
                                                </option>
                                            ))}
                                        </FormSelect>
                                    </div>
                                    <div className="col-6">
                                        <FormSelect
                                            key={`layoutConfigRight`}
                                            id={`layoutConfigRight`}
                                            name={`layoutConfigRight`}
                                            className="form-select-sm sm-1"
                                            error={{}}
                                            value={buttonLayouts.displayLayouts.buttonLayoutRightId}
                                            onChange={(e) => {}}
                                        >
                                            {Object.keys(buttonLayoutDefinitions.buttonLayoutRight).map((o, i) => (
                                                <option
                                                    key={o}
                                                    value={buttonLayoutDefinitions.buttonLayoutRight[o]}
                                                >
                                                    {t(`LayoutConfig:layouts.right.${o}`)}
                                                </option>
                                            ))}
                                        </FormSelect>
                                    </div>
                                </div>
                                <div className="d-flex led-preview-container">
                                    <div
                                        className={`led-preview`}
                                        onContextMenu={(e) => e.preventDefault()}
                                    >
                                        <div className="container-aux">
                                            {AUX_BUTTONS.map((buttonName) => (
                                                <LEDButton
                                                    key={`led-button-${buttonName}`}
                                                    className={`${buttonName} ${
                                                        selectedButton === buttonName ? 'selected' : ''
                                                    }`}
                                                    name={BUTTONS[buttonLabelType][buttonName]}
                                                    /*buttonColor={customTheme[buttonName]?.normal}
                                                    buttonPressedColor={customTheme[buttonName]?.pressed}*/
                                                    labelUnder={true}
                                                    onClick={(e) => toggleSelectedButton(e, buttonName)}
                                                />
                                            ))}
                                        </div>
                                        <div className="container-main">
                                            {buttonLayouts.displayLayouts.buttonLayout ? Object.keys(buttonLayouts.displayLayouts.buttonLayout).map((buttonID) => (
                                                <LEDButton
                                                    key={`led-button-left-${buttonID}`}
                                                    /*className={`${buttonName} ${
                                                        selectedButton === buttonName ? 'selected' : ''
                                                    }`}*/
                                                    name={BUTTONS[buttonLabelType][buttonLayouts.displayLayouts.buttonLayout[buttonID].elementType]}
                                                    elementProperties={buttonLayouts.displayLayouts.buttonLayout[buttonID]}
                                                    /*buttonColor={customTheme[buttonName]?.normal}
                                                    buttonPressedColor={customTheme[buttonName]?.pressed}*/
                                                    labelUnder={false}
                                                    onClick={(e) => toggleSelectedButton(e, buttonLayouts.displayLayouts.buttonLayout[buttonID].elementType)}
                                                />
                                            )) : ''}
                                            {buttonLayouts.displayLayouts.buttonLayoutRight ? Object.keys(buttonLayouts.displayLayouts.buttonLayoutRight).map((buttonID) => (
                                                <LEDButton
                                                    key={`led-button-right-${buttonID}`}
                                                    /*className={`${buttonName} ${
                                                        selectedButton === buttonName ? 'selected' : ''
                                                    }`}*/
                                                    name={BUTTONS[buttonLabelType][buttonLayouts.displayLayouts.buttonLayoutRight[buttonID].elementType]}
                                                    elementProperties={buttonLayouts.displayLayouts.buttonLayoutRight[buttonID]}
                                                    /*buttonColor={customTheme[buttonName]?.normal}
                                                    buttonPressedColor={customTheme[buttonName]?.pressed}*/
                                                    labelUnder={false}
                                                    onClick={(e) => toggleSelectedButton(e, buttonLayouts.displayLayouts.buttonLayoutRight[buttonID].elementType)}
                                                />
                                            )) : ''}
                                        </div>
                                    </div>
                                </div>
                            </Section>
                            <Button type="submit">{t('Common:button-save-label')}</Button>
                            {saveMessage ? (
                                <span className="alert">{saveMessage}</span>
                            ) : null}
                            <FormContext/>
                        </Form>
                    </div>
                )
            }
        </Formik>
    );
}
