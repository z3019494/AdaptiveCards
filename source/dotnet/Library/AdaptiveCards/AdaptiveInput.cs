// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using Newtonsoft.Json;
using System;
using System.ComponentModel;
using System.Xml.Serialization;

namespace AdaptiveCards
{
    /// <summary>
    ///     Input is used as part of a Form CardElement to collect information from a user
    /// </summary>
    public abstract class AdaptiveInput : AdaptiveElement
    {
        /// <summary>
        ///     The input must have a value for it to be part of a Submit action
        /// </summary>
        [JsonProperty(DefaultValueHandling = DefaultValueHandling.Ignore)]
#if !NETSTANDARD1_3
        [XmlAttribute]
        [DefaultValue(false)]
#endif
        public bool IsRequired { get; set; }

        /// <summary>
        ///     Error message displayed if input is not valid at submit
        /// </summary>
        [JsonProperty(NullValueHandling = NullValueHandling.Ignore)]
#if !NETSTANDARD1_3
        [XmlAttribute]
#endif
        public string ErrorMessage { get; set; }

        /// <summary>
        /// The string that will be rendered on a a host with "SupportsInteractivity" set to false.
        /// </summary>
        public abstract string GetNonInteractiveValue();
    }
}
