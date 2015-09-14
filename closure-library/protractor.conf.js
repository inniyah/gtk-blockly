// See https://github.com/angular/protractor/blob/master/docs/referenceConf.js
// for full protractor config reference.
exports.config = {
  sauceUser: process.env.SAUCE_USERNAME,

  sauceKey: process.env.SAUCE_ACCESS_KEY,

  // Options specific to which browser tests are run on.
  capabilities: {
    'browserName': 'chrome',
    'tunnel-identifier': process.env.TRAVIS_JOB_NUMBER,
    'build': process.env.TRAVIS_BUILD_NUMBER,
    'name': process.env.TRAVIS_PULL_REQUEST == 'false' ?
                'CO-' + process.env.TRAVIS_BRANCH + '-' +
                    process.env.TRAVIS_COMMIT :
                'PR-' + process.env.TRAVIS_PULL_REQUEST + '-' +
                    process.env.TRAVIS_BRANCH + '-' + process.env.TRAVIS_COMMIT
  },

  // Testing framework used for spec file.
  framework: 'jasmine2',

  // Relative path to spec (i.e., tests).
  specs: ['protractor_spec.js'],

  jasmineNodeOpts: {
    // Timeout in ms before a test fails. 30 minutes.
    defaultTimeoutInterval: 30 * 60 * 1000
  }
};
