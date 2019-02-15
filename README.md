# IDAO-2019-muon-id

This repository is the solution for the Online Round of [IDAO-2019](https://idao.world/) ML-contest of team **AR_U_KIDDIN_MI**.

Team members:
* [Eugen Bobrov](https://github.com/eugenbobrov) (leader);
* [Vladimir Bugaevskii](https://github.com/vbugaevskii);
* [Denis Bibik](https://github.com/den-bibik).

Baseline solution from contest's organizers can be found [here](https://github.com/yandexdataschool/IDAO-2019-muon-id).

## Track 1

| Notebook | Description |
|---|---|
| [`Features-FOI.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Features-FOI.ipynb) | Notebook contains engineering of advanced features based on FOI features. |
| [`Track1-MLPClassifier-fit.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-MLPClassifier-fit.ipynb) | Notebook contains feature engineering and fitting processes for an ensemble of Multilayer Perceptron Classifiers. |
| [`Track1-MLPClassifier-predict-public.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-MLPClassifier-predict-public.ipynb) | Notebook contains prediction process for an ensemble of Multilayer Perceptron Classifiers for public test dataset. |
| [`Track1-MLPClassifier-predict-private.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-MLPClassifier-predict-private.ipynb) | Notebook contains prediction process for an ensemble of Multilayer Perceptron Classifiers for private test dataset. |
| [`Track1-CatBoostClassifier-fit.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-CatBoostClassifier-fit.ipynb) | Notebook contains feature engineering and fitting processes for an ensemble of CatBoostClassifiers. |
| [`Track1-CatBoostClassifier-predict-public.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-CatBoostClassifier-predict-public.ipynb) | Notebook contains prediction process for an ensemble of CatBoostClassifiers for public test dataset. |
| [`Track1-CatBoostClassifier-predict-private.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-CatBoostClassifier-predict-private.ipynb) | Notebook contains prediction process for an ensemble of CatBoostClassifiers for private test dataset. |
| [`Track1-PredictionsMerge.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track1-PredictionsMerge.ipynb) | Notebook contains a weighted merge of predictions done by ensembles of MLPClassifiers and CatBoostClassifiers. |

More information about HEP MLPClassifier can be found [here](https://github.com/arogozhnikov/hep_ml).

## Track 2

[`Track2-CatBoostClassifier.ipynb`](https://github.com/vbugaevskii/IDAO-2019-muon-id/blob/master/Track2-CatBoostClassifier.ipynb) contains feature engineering and fitting model processes.

[`Track2-CPP`](https://github.com/vbugaevskii/IDAO-2019-muon-id/tree/master/Track2-CPP) contains C++ code for submission.

**Note:** Final submission for track 2 is the advanced baseline submission. CatBoostClassfier's parameters:
```json
{
  "iterations": 1600,
  "max_depth": 5
}
```
